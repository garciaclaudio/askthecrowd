{% load i18n %}

<div id="fb-root"></div>

<script>

var images_base_url = 'https://five-votes.appspot.com';
{% if IN_DEV_SERVER %}
    images_base_url = 'http://localhost:8080';
{% endif %}

var new_question_selected_pic_key = '';
var new_question_text = '';
var new_question_key_name = '';

function bind_delete_and_uploader(data) {
     // bind "add" click (form submit)
    $('#delete_answer_form_' + data.answer_key ).ajaxForm({
         dataType:  'json', 
         success: function(data) { 
             if( data.error ) {
                alert( data.error );
             } else {
                $('#answer_' + data.deleted_answer_key).remove();
             }
         }
    });

     // setup upload thingy
    var uploader = new qq.FileUploader({
            element: document.getElementById('file_uploader_' + data.answer_key),
            action: '/ajax.html',
            debug: true,
            sizeLimit: 1048576, // 1 MB
            uploadText: '{% trans "add picture" %}',
            allowedExtensions: ['jpg', 'jpeg', 'gif', 'png'],
            onComplete: function(id, file_name, data){
                $('#file_uploader_' + data.answer_key).remove();
                $('#picture_' + data.answer_key).html( $("#show_pic_tmpl").tmpl({'answer_key': data.answer_key} ));

                // save for feed posting
                if( ! new_question_selected_pic_key ) {
                     new_question_selected_pic_key = data.answer_key;
                }
            },
            params: {
                      action: 'upload_picture',
		      answer_key : data.answer_key,
            }
    });
}


function show_new_question() {
    $('#main_container').html( $("#new_question_tmpl").tmpl({}));
    $('#main_container').fadeIn('fast');
    $('#question_form').ajaxForm({
         dataType:  'json', 
         success: function(data) { 
             if( data.error ) {
                 alert( data.error );
             } else {
                 show_created_question(data);
             }
         }
     });
    $('#question').focus();

    // 
    $.getJSON("ajax.html?action=get_user_questions",
             function(data){
                 $('#prev_questions').html( $("#prev_questions_header_tmpl").tmpl({}));
                 for (var i = 0; i < data.length; i++) {
                     $('#prev_questions').append(
                          $("#prev_questions_tmpl").tmpl( {
                               'question_key_name': data[i].question_key_name,
                               'question_text': data[i].question_text,
                          }));                    
                 }
             }
    );
}



function show_answer(data) {
    $('#answers_div').append( $("#show_answer_tmpl").tmpl( {
       'answer_key': data.answer_key,
       'answer_text': data.answer_text,
    }));

    bind_delete_and_uploader(data);
}

function show_created_question(data) {

    // save for feed posting
    new_question_text = data.question_text;
    new_question_key_name = data.question_key_name;

    $('#main_container').fadeOut('fast');
    $('#main_container').html( $("#question_created_tmpl").tmpl(
       {
         'question_text': data.question_text,
         'question_key_name': data.question_key_name,
       }));

     // bind "add" click (form submit)
     $('#new_answer_form').ajaxForm({
         dataType:  'json', 
         success: function(data) { 
             if( data.error ) {
                 alert( data.error );
             } else {

                //clean answer form
                $('#new_answer').val('');
                show_answer(data);
             }
         }
      });

      $('#new_answer').focus();

    $('#done_button').click( function() {

        // post to fb if requested
        if( $('input[name=post_to_fb]').is(':checked') ) {
            var pic_link = 'https://five-votes.appspot.com/images/open_letter.jpg';
            if( new_question_selected_pic_key ) {
                pic_link = images_base_url + '/image?answer_key=' + new_question_selected_pic_key
                new_question_selected_pic_key = '';
            }

            name = new_question_text;
            caption = "{{user_name}} " + ' {% trans "created this question on Five Votes." %}';
            var desc = '{% trans "Your opinion is needed!" %} ';

            var obj = {
              method: 'feed',
              link: 'https://five-votes.appspot.com/q' + new_question_key_name,
              picture: pic_link,
              name: name,
              caption: caption,
              description: desc,
            };
            FB.ui(obj);
        }

        $('#main_container').fadeOut('fast');
        $('#main_container').html( $("#done_question_tmpl").tmpl({
             'question_text': data.question_text,
             'question_key_name': data.question_key_name,
           }));
           // bind add-another button
           $('#create_another_button').click( function() {
               show_new_question();
           });

        $('#main_container').fadeIn('fast');


    });

    $('#main_container').fadeIn('fast');
}


function update_votes_header( votes_left ) {
    if( votes_left == 0 ) {
       $('#app_logo').html('<div class="mytube">{% trans "Zero Votes" %}</div>');
    }
    else if( votes_left == 1 ) {
       $('#app_logo').html('<div class="mytube">{% trans "One Vote" %}</div>');
    }
    else if( votes_left == 2 ) {
       $('#app_logo').html('<div class="mytube">{% trans "Two Votes" %}</div>');
    }
    else if( votes_left == 3 ) {
       $('#app_logo').html('<div class="mytube">{% trans "Three Votes" %}</div>');
    }
     else if( votes_left == 4 ) {
       $('#app_logo').html('<div class="mytube">{% trans "Four Votes" %}</div>');
    }
     else if( votes_left == 5 ) {
       $('#app_logo').html('<div class="mytube">{% trans "Five Votes" %}</div>');
    }
}


var answer_votes = {}

function show_votes( answer_key, num_votes ) {
    answer_votes[answer_key] = num_votes;
    // change color of answer card
    $("#answer_" + answer_key).addClass('voted_answer');
    // show / update value of count_inset div
    $("#count_inset_" + answer_key).show();
    $("#count_text_" + answer_key).html( num_votes + '&#10003;' );
    // make unvote link appear
    $("#minus_vote_link_" + answer_key).show();
    // update votes left counter (the header div)
}


function show_data( data, id ) {

    $('#result_answers_div').html('');

    for (var i = 0; i < data.results[id].length; i++) {
        ans_key = data.results[id][i][0];
        num_votes = data.results[id][i][1];
        ans = data.answers_hash[ans_key];

        $('#result_answers_div').append( $("#show_answer_for_results_tmpl").tmpl( {
            'answer_key' : ans.answer_key,
            'answer_text' : ans.answer_text,
            'num_votes' : num_votes,
         }));

        if( ans.has_pic ) {
           $('#picture_' + ans.answer_key ).html( $("#show_result_pic_tmpl").tmpl({'answer_key': ans.answer_key}));
        }               
    }
}


var selected_tab = 'all';
function show_selector( data, id, text, selected ) {
     if( selected ) {
         text_visible = 'visible';
         link_visible = 'hidden';
         selected_result = 'selected_result';
     } else  {
         text_visible = 'hidden';
         link_visible = 'visible';
         selected_result = '';
     }

     $('#selector_div').append( $("#res_selector_tmpl").tmpl( {
            'id' : id,
            'text' : text,
            'text_visible' : text_visible,
            'link_visible' : link_visible,
            'selected_result' : selected_result,
     }));

     $('#selector_link_' + id).click(function() {
          $("#selector_" + selected_tab).removeClass('selected_result');
          $("#selector_link_" + selected_tab).show();
          $("#selector_text_" + selected_tab).hide();
          $("#selector_" + id).addClass('selected_result');
          $('#selector_link_' + id).hide();
          $('#selector_text_' + id).show();
          selected_tab = id;
          show_data( data, id );
     } );
}


function show_results( data ) {
    $('#main_container').fadeOut('fast');

    $('#main_container').html( $("#show_results_tmpl").tmpl({
            'question_text': data.question_text,
            'question_key_name': data.question_key_name,
          }));

    show_selector(data, 'all', '{% trans "All" %}', 1 );
    show_selector(data, 'male', '{% trans "Women" %}', 0 );
    show_selector(data, 'female', '{% trans "Men" %}', 0 );

    for (var i = 0; i < data.friends_with_votes.length; i++) {
        friend_id = 'friend_' + data.friends_with_votes[i].user_id;
        show_selector(data, friend_id, data.friends_with_votes[i].name, 0 );
    }

    show_data( data, 'all');

// foreach all, women, men, friend list
//   add and bind a result_selector link
//   "all" is selected by default
//   have a global var keeping track of what's selected
//   when something new is selected:
//      (hide link, show span) of new, unselect previous (hide span, show link)

    $('#main_container').fadeIn('fast');
}



function bind_vote_links(answer_key) {

     $('#plus_vote_form_' + answer_key ).ajaxForm({
         dataType:  'json', 
         success: function(data) { 
             if( data.error ) {
                alert( data.error );
             } else {
                if( data.votes_left <= 0 ) {
                   $('.plus_vote_div').hide();
                   $('#see_results_div').show();
                }
                $('#del_ans_link_' + data.answer_key).hide();
                show_votes( data.answer_key, data.new_count );
                update_votes_header( data.votes_left );
             }
         }
     });

     $('#minus_vote_form_' + answer_key ).ajaxForm({
         dataType:  'json', 
         success: function(data) { 
             if( data.error ) {
                alert( data.error );
             } else {

                update_votes_header( data.votes_left );

                if( data.votes_left > 0 ) {
                   $('.plus_vote_div').show();
                   $('#see_results_div').hide();
                }
                if( data.new_count == 0 ) {
                    $('#del_ans_link_' + data.answer_key).show();
                    $("#count_inset_" + data.answer_key).hide();
                    $("#answer_" + data.answer_key).removeClass('voted_answer');
                    $("#minus_vote_link_" + data.answer_key).hide();
                } else {
                    $("#count_text_" + data.answer_key).html( data.new_count + '&#10003;' );
                }
             }
         }
     });
}




var answers = {};

function show_question_for_voting() {

     if( '{{votes_left}}' == 0 ) {
        $.getJSON("ajax.html?action=get_results;question_key_name={{question_key_name}}",
             function(data){
                 show_results(data);
             }
        );
        return;
     }

    $('#main_container').html( $("#answer_question_tmpl").tmpl({
             'question_text': "{{question.question_text}}",
             'question_key_name': "{{question_key_name}}",
	     }));
             
    $('#see_results_button').click( function() {

        if( $('input[name=post_to_fb]').is(':checked') ) {
            var selected_answer_text;
            var selected_answer_key;
            var selected_ans_votes = 0;
            for (answer_key in answer_votes) {
                n_votes = answer_votes[answer_key];
                answer = answers[answer_key];
                if(n_votes > selected_ans_votes) {
                    selected_answer_text = answer.answer_text;
                    if( answer.has_pic > 0 ) {
                        selected_ans_votes = n_votes;
                        selected_answer_key = answer_key;
                    }
                }
            }

            var pic_link = 'https://five-votes.appspot.com/images/open_letter.jpg';
            if( selected_ans_votes > 0 ) {
                pic_link = images_base_url + '/image?answer_key=' + selected_answer_key
            }

            name = "{{question.question_text}}";
            caption = "{{user_name}} " + ' {% trans "answered this question on Five Votes." %}';

{% if  user_is_male %}
            var desc = '{% trans "His selection was:" %} ' + selected_answer_text;
{% else %}
            var desc = '{% trans "Her selection was:" %} ' + selected_answer_text;
{% endif %}

            var obj = {
              method: 'feed',
              link: 'https://five-votes.appspot.com/q{{question_key_name}}',
              picture: pic_link,
              name: name,
              caption: caption,
              description: desc,
            };
            function callback(response) {
                $.getJSON("ajax.html?action=get_results;question_key_name={{question_key_name}}",
                   function(data){
                      show_results(data);
                   }
                );
            }
            FB.ui(obj, callback);
        } else {
            $.getJSON("ajax.html?action=get_results;question_key_name={{question_key_name}}",
                 function(data){
                     show_results(data);
                 }
            );
        }
    });

    {% for ans in answers %}

      var ans = { 'has_pic' : '{{ans.has_pic}}',
                  'answer_key' : '{{ans.answer_key}}',
                  'answer_text' : '{{ans.answer_text}}', };

      answers[ans.answer_key] = ans;

      $('#answers_div').append( $("#show_answer_for_voting_tmpl").tmpl( {
         'answer_key' : '{{ans.answer_key}}',
         'answer_text' : '{{ans.answer_text}}',
      }));

      {% if ans.has_pic %}
          $('#picture_' + '{{ans.answer_key}}').html( $("#show_pic_tmpl").tmpl({'answer_key': '{{ans.answer_key}}'} ));
      {% endif %}

      {% if ans.num_votes %}
          show_votes( '{{ans.answer_key}}', {{ans.num_votes}} );
      {% endif %}

      bind_vote_links('{{ans.answer_key}}');

    {% endfor %}

    update_votes_header( {{votes_left}} );

    {% if votes_left <= 0 %}
        $('.plus_vote_div').hide();
        $('#see_results_div').show();
    {% endif %}

    // add and bind new_answer_form here
}



function show_question_for_welcome() {

    $('#main_container').html( $("#welcome_question_tmpl").tmpl({
             'question_text': "{{question.question_text}}",
             'question_key_name': "{{question_key_name}}",
	     }));

    {% for ans in answers %}
      $('#answers_div').append( $("#show_answer_for_welcome_tmpl").tmpl( {
         'answer_key' : '{{ans.answer_key}}',
         'answer_text' : '{{ans.answer_text}}',
      }));

      {% if ans.has_pic %}
          $('#picture_' + '{{ans.answer_key}}').html( $("#show_pic_tmpl").tmpl({'answer_key': '{{ans.answer_key}}'} ));
      {% endif %}
    {% endfor %}
}


function show_answer_for_voting_and_editing(data) {
    $('#answers_div').append( $("#show_answer_for_voting_and_editing_tmpl").tmpl( {
       'answer_key': data.answer_key,
       'answer_text': data.answer_text,
    }));

    bind_delete_and_uploader(data);
    bind_vote_links(data.answer_key);
}


function add_ans_link_func() {
    $('#add_ans_link').hide();

    // bind "add" click (form submit)
    $('#add_new_ans_form').ajaxForm({
         dataType:  'json', 
         success: function(data) { 
             if( data.error ) {
                 alert( data.error );
             } else {

                //clean answer form
                $('#new_answer').val('');
                show_answer_for_voting_and_editing(data);
                $('#add_new_ans').hide();
                $('#add_ans_link').show();
             }
         }
    });

    $('#add_new_ans').show();
    $('#new_answer').focus();
}



{% if NOT_IN_DEV_SERVER %}

        window.fbAsyncInit = function() {
          FB.init({
            appId      : '331909936825023',
            channelUrl : '//five-votes.appspot.com/channel.html',
            status     : true, 
            cookie     : true,
            xfbml      : true
          });

          //
          // when the user clicks the login button, this event is fired,
          // response contains the user data, but we prefer to reload
          // so that a user is created
          //
         {% if question %}
              FB.Event.subscribe('auth.login', function(response) {
                  // Reload the entire page
                  top.location = 'http://apps.facebook.com/five_votes/q{{question_key_name}}';
              });
         {% else %}
              FB.Event.subscribe('auth.login', function(response) {
                  // Reload the entire page
                  top.location = 'http://apps.facebook.com/five_votes/';
              });
         {% endif %}
//
//          FB.ui({ method: 'feed', 
//                  message: 'Facebook for Websites is super-cool'});

            FB.getLoginStatus(function(response) {

              if (response.status === 'connected') {
                  $('#faces_div').html( $("#logged_in_faces_tmpl").tmpl({}) );
                  FB.XFBML.parse( document.getElementById('faces_div') );

                  // the user is logged in and connected to your
                  // app, and response.authResponse supplies
                  // the user's ID, a valid access token, a signed
                  // request, and the time the access token 
                  // and signed request each expire
                  var uid = response.authResponse.userID;
                  var accessToken = response.authResponse.accessToken;

                  {% if question %}
                      show_question_for_voting();
                  {% else %}
                      show_new_question();
                  {% endif %}

//              } else if (response.status === 'not_authorized') {
//                  // the user is logged in to Facebook, 
//                  //but not connected to the app
//
//                  $('#main_container').html( $("#welcome_msg_tmpl").tmpl({}));
//                  FB.XFBML.parse( document.getElementById('welcome_msg') );
              } else {
                // the user isn't even logged in to Facebook.

                  {% if question %}
                      show_question_for_welcome();
                  {% else %}
                      $('#main_container').html( $("#welcome_msg_tmpl").tmpl({}));
                  {% endif %}
            
                  FB.XFBML.parse( document.getElementById('welcome_msg') );
            
              }
            
            });
        };

{% endif %}

        (function(d){
           var js, id = 'facebook-jssdk'; if (d.getElementById(id)) {return;}
           js = d.createElement('script'); js.id = id; js.async = true;
           js.src = "//connect.facebook.net/en_US/all.js";
           d.getElementsByTagName('head')[0].appendChild(js);
         }(document));

</script>

<script type="text/javascript">

$(function(){

{% if IN_DEV_SERVER %}

    {% if question %}

        show_question_for_voting();

    {% else %}
       show_new_question();
    {% endif %}
{% endif %}


});

</script>


