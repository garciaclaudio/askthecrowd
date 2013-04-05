<div id="fb-root"></div>

<script>

{% if question is defined %}
    {% include "templates/control_question.inc" %}
{% endif %}

//### XXX not used, may want to fix
jQuery.fn.idle = function(time){
	var i = $(this);
	i.queue(function(){
		setTimeout(function(){
			i.dequeue();
		}, time);
	});
};


// XXX, change this when going live
{% if IN_DEV_SERVER %}
var images_base_url = 'http://localhost:8080';
{% else %}
var images_base_url = 'http://five-votes.appspot.com';
{% endif %}

var logo_pic_link = 'http://five-votes.appspot.com/images/Logo_5votes_small.jpg';



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

    if( ! data.has_pic && data.video_id == 'None' ) {

        // show video link
        $('#add_video_link_' + data.answer_key).show();
        // setup upload thingy
        var uploader = new qq.FileUploader({
                element: document.getElementById('file_uploader_' + data.answer_key),
                action: '/ajax.html',
                debug: true,
                sizeLimit: 2*2088576, // 4 MB
                uploadText: '{% trans %}add picture{% endtrans %}',
                allowedExtensions: ['jpg', 'jpeg', 'gif', 'png'],
                onComplete: function(id, file_name, data){
                    $('#file_uploader_' + data.answer_key).remove();
                    $('#picture_' + data.answer_key).html( $("#show_pic_tmpl").tmpl({'answer_key': data.answer_key} ));
                    // pic uploaded, do a masonry reload when the image is loaded!
                    var img = new Image();
                    img.onload = function() {
                        $(window).scrollTop($('#picture_' + data.answer_key ).offset().top - 100);
                       $('#answer_' + data.answer_key).parent().masonry({itemSelector: '.answer_card', isFitWidth: true
                        });

                    }
                    img.src = "image?answer_key=" + data.answer_key;

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
}


var existing_usr_questions = {}
var question_key_name_to_delete = ''

function get_and_display_user_questions() {
    // 
    $.getJSON("ajax.html?action=get_user_questions",
             function(data){
                 $('#prev_questions').html( $("#prev_questions_header_tmpl").tmpl({}));

                 $( "#dialog-confirm" ).dialog({
                             autoOpen: false,
                             resizable: false,
                             height:220,
                             modal: true,
                             buttons: {
				"{% trans %}yes, delete it{% endtrans %}": function() {

                                    $.getJSON("ajax.html?action=delete_question;question_key_name=" + question_key_name_to_delete, function(data){
                                            $('#prev_question_' + data.deleted_question_key).hide();
                                    });

                                    $( this ).dialog( "close" );
                                    return false;
				},
				"{% trans %}no, cancel{% endtrans %}": function() {
					$( this ).dialog( "close" );
                                        return false;
				}
                             }
                 });

                 for (var i = data.length-1; i >= 0; i--) {
//alert( i + '--' + data[i].question_text + '--' + data.length);
                     q = data[i];

                     existing_usr_questions[q.question_key_name] = q.question_text;

                     $('#prev_questions').append(
                          $("#prev_questions_tmpl").tmpl( {
                               'question_key_name': q.question_key_name,
                               'question_text': q.question_text,
                     }));

                     $('#more_link_' + q.question_key_name).click( function() {
                              var key = $(this).attr('q_key');
                              $('#prev_answers_div_' + key).show();
                              $('#more_link_' + key).hide();
                              $('#less_link_' + key).show();
                              $('#inner_prev_answers_div_' + key ).masonry({
                                   itemSelector: '.answer_card',
                                   isFitWidth: true
                              });
                      });

                      $('#less_link_' + q.question_key_name).click( function() {
                              var key = $(this).attr('q_key');
                              $('#prev_answers_div_' + key).hide();
                              $('#more_link_' + key).show();
                              $('#less_link_' + key).hide();
                      });

                      var ans = q.answers;
                      for (var j = 0; j < ans.length; j++) {
//                        alert( q.question_text + ' -- has pic:' + ans[j].has_pic );
                          $('#inner_prev_answers_div_' + q.question_key_name).append(
                              $("#show_answer_for_prev_questions_tmpl").tmpl( {
                                   'answer_key': ans[j].answer_key,
                                   'answer_text': ans[j].answer_text,
                                   'has_pic': ans[j].has_pic,
                                   'video_id': ans[j].video_id,
                                   'num_votes': ans[j].num_votes,
                                   'visible': (ans[j].num_votes>0) ? '' : 'invisible',
                              }));
                          bind_delete_and_uploader(ans[j]);
                         if( ans[j].has_pic ) {
                             $('#picture_' + ans[j].answer_key ).html( $("#show_pic_tmpl").tmpl({'answer_key': ans[j].answer_key}));
                         }
                         if( ans[j].video_id != 'None') {
                             $('#picture_' + ans[j].answer_key ).html( $("#show_video_tmpl").tmpl({'answer_key': ans[j].answer_key, 'video_id':ans[j].video_id}));
                             $('#play_button_' + ans[j].answer_key ).show();
                             $('#play_button_' + ans[j].answer_key + ' > a').attr('video_id', ans[j].video_id);
                             $('#play_button_' + ans[j].answer_key + ' > a').click(  function() { 
                                 var vid_id = $(this).attr('video_id');
                                 $('#video_link_' + vid_id).trigger('click');
                             });
                         }


                      }
                 }
                 $('.delete_question_button').click( function() {
                         var key = $(this).attr('q_key');
                         question_key_name_to_delete = key;
                         $('#confirm_delete_question_div').html(existing_usr_questions[key]);
                         $( "#dialog-confirm" ).dialog('open');

                 });
             }
    );
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
    get_and_display_user_questions();
}



function show_answer(data) {

    $('#answers_div').append( $("#show_answer_tmpl").tmpl( {
       'answer_key': data.answer_key,
       'answer_text': data.answer_text,
    }));

    bind_delete_and_uploader(data);

    $('#answers_div').masonry('reload');
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
            var pic_link = logo_pic_link
            if( new_question_selected_pic_key ) {
                pic_link = images_base_url + '/image?answer_key=' + new_question_selected_pic_key
                new_question_selected_pic_key = '';
            }

//alert('pic link: ' + pic_link );

            name = new_question_text;
            caption = "{{user_name}} " + ' {% trans %}created this question on Five Votes.{% endtrans %}';
            var desc = '{% trans %}Your opinion is needed!{% endtrans %} ';

            var obj = {
              method: 'feed',
              link: '{{BASE_URL}}' + '/q' + new_question_key_name,
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

        // this is wasteful replace, wait to allow for repl. delay
        setTimeout(get_and_display_user_questions, 1500) 

        $('#main_container').fadeIn('fast');
    });

    $('#main_container').fadeIn('fast');
}


function update_votes_header( votes_left ) {
    if( votes_left == 0 ) {
       $('#logo_text').html('<div class="mytube">{% trans %}Zero Votes{% endtrans %}</div>');
    }
    else if( votes_left == 1 ) {
       $('#logo_text').html('<div class="mytube"><span style="color:red;">&#10003;</span> {% trans %}One Vote{% endtrans %}</div>');
    }
    else if( votes_left == 2 ) {
       $('#logo_text').html('<div class="mytube"><span style="color:red;">&#10003;</span> {% trans %}Two Votes{% endtrans %} <span style="color:red;">&#10003;</span> </div>');
    }
    else if( votes_left == 3 ) {
       $('#logo_text').html('<div class="mytube"><span style="color:red;">&#10003;&#10003;</span> {% trans %}Three Votes{% endtrans %} <span style="color:red;">&#10003;</span> </div>');
    }
     else if( votes_left == 4 ) {
       $('#logo_text').html('<div class="mytube"><span style="color:red;">&#10003;&#10003;</span> {% trans %}Four Votes{% endtrans %} <span style="color:red;">&#10003;&#10003;</span> </div>');
    }
     else if( votes_left == 5 ) {
       $('#logo_text').html('<div class="mytube"><span style="color:red;">&#10003;&#10003;&#10003;</span> {% trans %}Five Votes{% endtrans %} <span style="color:red;">&#10003;&#10003;</span> </div>');
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


function dynamicSort(property) {
    return function (a,b) {
        return (b[property] < a[property]) ? -1 : (b[property] > a[property]) ? 1 : 0;
    }
}

function _set_results_width() {
//    var parent_width = $(window).width();
    var parent_width = $('#main_container').width();
    if( parent_width > 400 ) {
       $('.results_card').width(450);
    } else {
       $('.results_card').width(parent_width-40);
    }
}

function show_data( data, id ) {

    $('#result_answers_div').html('');

    results = data.results[id];

    results.sort(dynamicSort(1));

    for (var i = 0; i < results.length; i++) {
        ans_key = results[i][0];
        num_votes = results[i][1];
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

    _set_results_width();
    $(window).on("debouncedresize", function( event ) {
        _set_results_width();
    });
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
            'owner_id' : data.owner_id,
            'owner_name' : data.owner_name,
            'question_text': data.question_text,
            'question_key_name': data.question_key_name,
          }));

    show_selector(data, 'all', '{% trans %}All{% endtrans %}', 1 );
    show_selector(data, 'female', '{% trans %}Women{% endtrans %}', 0 );
    show_selector(data, 'male', '{% trans %}Men{% endtrans %}', 0 );

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
         beforeSubmit: function(data) { 
            $("#count_inset_" + answer_key).hide();
            $("#whirly_" + answer_key).show();
         },
         success: function(data) { 
             if( data.error ) {
                alert( data.error );
             } else {
                if( data.votes_left <= 0 ) {
                   $('.plus_vote_div').hide();
                   $('#see_results_div').show();

                    $('html, body').animate({
                        scrollTop: $("#see_results_div").offset().top - 100
                    }, 2000);

                   $('.add_ans_link').hide();
                   $('#add_new_ans').hide();
                }
                $('#del_ans_link_' + data.answer_key).hide();
                $("#count_inset_" + answer_key).show();
                $("#whirly_" + answer_key).hide();
                show_votes( data.answer_key, data.new_count );
                update_votes_header( data.votes_left );
             }
         }
     });

     $('#minus_vote_form_' + answer_key ).ajaxForm({
         dataType:  'json', 
         beforeSubmit: function(data) { 
            $("#count_inset_" + answer_key).hide();
            $("#whirly_" + answer_key).show();
         },
         success: function(data) { 
             if( data.error ) {
                alert( data.error );
             } else {
                $("#whirly_" + data.answer_key).hide();
                $("#count_inset_" + data.answer_key).show();
                update_votes_header( data.votes_left );

                if( data.votes_left > 0 ) {
                   $('.plus_vote_div').show();
                   $('#see_results_div').hide();
                   $('.add_ans_link').show();
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





function show_answer_for_voting_and_editing(data) {

    var new_elem =  $("#show_answer_for_voting_and_editing_tmpl").tmpl( {
       'answer_key': data.answer_key,
       'answer_text': data.answer_text,
       'owner_id' : data.owner_id,
       'owner_name' : data.owner_name,
    });

    $('#answers_div').append( new_elem );

    $('#answers_div').masonry('reload');

    bind_delete_and_uploader(data);
    bind_vote_links(data.answer_key);
}


function add_ans_link_func() {
    $('.add_ans_link').hide();

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
                $('.add_ans_link').show();
             }
         }
    });

    $('#add_new_ans').show();
    $('#new_answer').focus();
}


function show_all_questions() {

     $('#all_users_questions').hide();

     $('#main_container').html( '<h3>{% trans %}Questions from all users{% endtrans %}</h3>' );

    {% for q in questions %}
      $('#prev_questions').append( $("#all_questions_question_tmpl").tmpl( {
         'question_key_name' : '{{q.question_key_name}}',
         'question_text' : '{{q.question_text}}',
      }));
    {% endfor %}

    {% for q in questions %}
//      $('#all_questions_div').append( $("#all_questions_question_tmpl").tmpl( {
//         'question_key' : '{{q.question_key}}',
//         'question_text' : '{{q.question_text}}',
//      }));
    {% endfor %}
}

function add_video_link_func( ans_key ) {

        $('#file_uploader_' + ans_key).hide();
        $('#add_video_link_' + ans_key).hide();
        $('#video_form_div_' + ans_key).show();
        $('#answer_' + ans_key).parent().masonry({itemSelector: '.answer_card', isFitWidth: true
                        });
        $('#add_video_form_' + ans_key ).ajaxForm({
             dataType:  'json', 
             success: function(data) { 
                 if( data.error ) {
                    alert( data.error );
                 } else {
                    $('#video_form_div_' + ans_key).hide();
                    $('#picture_' + data.answer_key).html( $("#show_video_tmpl").tmpl( {'answer_key': data.answer_key, 'video_id': data.video_id} ));

                    var img = new Image();
                    img.onload = function() {
                       $('#answer_' + data.answer_key).parent().masonry({itemSelector: '.answer_card', isFitWidth: true
                        });
                    }
                    img.src = "http://img.youtube.com/vi/" + data.video_id  +  "/mqdefault.jpg";
                    $('#picture_' + data.answer_key ).html( $("#show_video_tmpl").tmpl({'answer_key': data.answer_key, 'video_id':data.video_id}));
                             $('#play_button_' + data.answer_key ).show();
                             $('#play_button_' + data.answer_key + ' > a').attr('video_id', data.video_id);
                             $('#play_button_' + data.answer_key + ' > a').click(  function() { 
                                 var vid_id = $(this).attr('video_id');
                                 $('#video_link_' + vid_id).trigger('click');
                             });
                 }
             }
        });
}

{% if NOT_IN_DEV_SERVER_OLD %}

        window.fbAsyncInit = function() {
          FB.init({
            appId      : '331909936825023',
//            channelUrl : '//five-votes.appspot.com/channel.html',
            status     : true, 
            cookie     : true,
            xfbml      : true
          });

          //
          // when the user clicks the login button, this event is fired,
          // response contains the user data, but we prefer to reload
          // so that a user is created
          //
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

//            alert('user connected ' +  uid + ' -- ' + accessToken );

//alert( 'NAME? {{logged_in_user.name}} <--' );

    {% if logged_in_user %}

//alert('make content visible...');

//$('#content_here').show();
    {% else %}

//alert('reloading, user not there');
          window.location.reload();
    {% endif %}

                  {% if question is defined %}
                      show_question_for_voting();
                      $('.answer_picture').imagesLoaded(
                          function(){ 
                              $('#answers_div').masonry({
                                   itemSelector: '.answer_card',
                                   isFitWidth: true
                              });
                           }
                      );

                  {% else %}

                      {% if all_questions %}
                           show_all_questions()
                      {% endif %}
                      {% if main_page %}
                          show_new_question();
                      {% endif %}

                  {% endif %}

//              } else if (response.status === 'not_authorized') {
//                  // the user is logged in to Facebook, 
//                  //but not connected to the app
//
//                  $('#main_container').html( $("#welcome_msg_tmpl").tmpl({}));
//                  FB.XFBML.parse( document.getElementById('welcome_msg') );
              } else {
                // the user isn't even logged in to Facebook.

                  {% if question is defined %}
                    FB.Event.subscribe('auth.login', function(response) {
                    // Reload the entire page
//alert('about to reload question');
                        top.location = '{{BASE_URL}}/q{{question_key_name}}';
                    });
                  {% else %}
                    FB.Event.subscribe('auth.login', function(response) {
                     // Reload the entire page
//alert('about to reload');
                        top.location = '{{BASE_URL}}';
                    });
                  {% endif %}

                  {% if question is defined %}
                      show_question_for_welcome();
                      $('.answer_picture').imagesLoaded(
                          function(){ 
                              $('#answers_div').masonry({
                                   itemSelector: '.answer_card',
                                   isFitWidth: true
                              });
                           }
                      );
                  {% else %}
                      $('#main_container').html( $("#welcome_msg_tmpl").tmpl({}));
                  {% endif %}
            
                  FB.XFBML.parse( document.getElementById('welcome_msg') );
                  FB.XFBML.parse( document.getElementById('second_login_button') );
            
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

$(function(){

//if( top.location.href === location.href ) {
//   alert('in standalone site');
//   base_url = 'http://five-votes.appspot.com';
//} else {
//   alert('in iframe');
//}

    $('.fancybox-media').fancybox({
        openEffect  : 'none',
        closeEffect : 'none',
        helpers : {
            media : {}
        }
    });

{% if IN_DEV_SERVER_OLD %}

    {% if question is defined %}
        show_question_for_voting();
    {% else %}
        {% if all_questions %}
            show_all_questions()
        {% endif %}
        {% if main_page %}
           show_new_question();
        {% endif %}
    {% endif %}
{% endif %}
});


$(window).load(function(){  
  $('#answers_div').masonry({
    itemSelector: '.answer_card',
    isFitWidth: true
  });
});


</script>

