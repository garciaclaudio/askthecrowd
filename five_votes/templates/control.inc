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


jQuery.extend({
    handleError: function( s, xhr, status, e ) {
        // If a local callback was specified, fire it
        if ( s.error )
            s.error( xhr, status, e );
        // If we have some XML response text (e.g. from an AJAX call) then log it in the console
        else if(xhr.responseText)
            alert('why is form generating an error?' + xhr.responseText );
//            console.log(xhr.responseText);
    }
});


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
                multiple: false,
                sizeLimit: 2*2088576, // 4 MB
                uploadText: '{% trans %}add picture{% endtrans %}',
                allowedExtensions: ['jpg', 'jpeg', 'gif', 'png'],
//                onSubmit: function(id, file_name){
//                   alert('id: ' + id + ' name: ' + file_name );
//                },
                onComplete: function(id, file_name, data){
                    $('#add_video_link_' + data.answer_key).remove();
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
                               'question_desc': q.question_desc,
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
                                   'link': ans[j].link,
                                   'has_pic': ans[j].has_pic,
                                   'video_id': ans[j].video_id,
                                   'num_votes': ans[j].num_votes,
                                   'visible': (ans[j].num_votes>0) ? '' : 'invisible',
                                   'show_add_link': ! ans[j].link && ! ans[j].video_id,
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
    $('#main_container').html( $("#new_question_tmpl").tmpl({
      {% if show_uploader %}
       'show_uploader': {{show_uploader}},
      {% endif %}
    }));
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
    $('#question_desc').autosize(); 
    get_and_display_user_questions();
}



function show_answer(data) {

    $('#answers_div').append( $("#show_answer_tmpl").tmpl( {
       'answer_key': data.answer_key,
       'answer_text': data.answer_text,
    }));

    if(data.video_id != 0) {
        $('#add_link_' + data.answer_key ).hide();
        $('#picture_' + data.answer_key ).html( $("#show_video_tmpl").tmpl({'answer_key': data.answer_key, 'video_id':data.video_id}));
        $('#play_button_' + data.answer_key ).show();
        $('#play_button_' + data.answer_key + ' > a').attr('video_id', data.video_id);
        $('#play_button_' + data.answer_key + ' > a').click(  function() { 
            var vid_id = $(this).attr('video_id');
            $('#video_link_' + vid_id).trigger('click');
        });
    }

    bind_delete_and_uploader(data);

    $('#answers_div').masonry('reload');
}

function show_created_question(data) {

    // save for feed posting
    new_question_text = data.question_text;
    new_question_desc = data.question_desc;
    new_question_key_name = data.question_key_name;

    $('#main_container').fadeOut('fast');

    $('#main_container').html( $("#question_created_tmpl").tmpl(
       {
         'question_text': data.question_text,
         'question_desc': data.question_desc,
         'question_key_name': data.question_key_name,
         'youtube_video_page': data.youtube_video_page,
       }));

     $('#new_answer').autosize();

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
             'question_desc': data.question_desc,
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
       $('#logo_text').html('{% trans %}Zero Votes{% endtrans %}');
    }
    else if( votes_left == 1 ) {
       $('#logo_text').html('<span style="color:red;">&#10003;</span> {% trans %}One Vote{% endtrans %}');
    }
    else if( votes_left == 2 ) {
       $('#logo_text').html('<span style="color:red;">&#10003;</span> {% trans %}Two Votes{% endtrans %} <span style="color:red;">&#10003;</span>');
    }
    else if( votes_left == 3 ) {
       $('#logo_text').html('<span style="color:red;">&#10003;&#10003;</span> {% trans %}Three Votes{% endtrans %} <span style="color:red;">&#10003;</span>');
    }
     else if( votes_left == 4 ) {
       $('#logo_text').html('<span style="color:red;">&#10003;&#10003;</span> {% trans %}Four Votes{% endtrans %} <span style="color:red;">&#10003;&#10003;</span>');
    }
     else if( votes_left == 5 ) {
       $('#logo_text').html('<span style="color:red;">&#10003;&#10003;&#10003;</span> {% trans %}Five Votes{% endtrans %} <span style="color:red;">&#10003;&#10003;</span>');
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


function dynamicSort(ans_hash) {
    return function (a,b) {

        ans_txt_b = ans_hash[ b[0] ].answer_text;
        ans_txt_a = ans_hash[ a[0] ].answer_text;

        return (b[1] < a[1]) ? -1 : (b[1] > a[1]) ? 1 : 
               (ans_txt_b < ans_txt_a) ? -1 : (ans_txt_b > ans_txt_a) ? 1 : 0;
    }
}

function _set_results_width() {

    var parent_width = $('#main_container').width();
    if( parent_width > 400 ) {
       $('.results_card').width(450);
    } else {
       $('.results_card').width(parent_width-40);
    }
}

function show_data( data, results ) {

    $('#result_answers_div').html('');

    // add answers with zero votes to results
    // make hash with result keys
    var results_hash = {};
    for (var i = 0; i < results.length; i++) {
        ans_key = results[i][0];
        results_hash[ans_key] = 1;
    }
    // go through all answers in data.answers_hash 
    for (var ans_key in data.answers_hash) {
        if (data.answers_hash.hasOwnProperty(ans_key)) {
            if( results_hash[ans_key] != 1 ) {
                results.push( [ans_key, 0] );
            }
        }
    }

    results.sort( dynamicSort(data.answers_hash) );

    for (var i = 0; i < results.length; i++) {
        ans_key = results[i][0];
        num_votes = results[i][1];
        ans = data.answers_hash[ans_key];


        $('#result_answers_div').append( $("#show_answer_for_results_tmpl").tmpl( {
            'link' : ans.link,
            'answer_key' : ans.answer_key,
            'answer_text' : ans.answer_text,
            'num_votes' : num_votes,
         }));

        if(ans.video_id != 'None') {

            $('#picture_' + ans.answer_key ).html( $("#show_video_tmpl").tmpl({'answer_key': ans.answer_key, 'video_id':ans.video_id}));
                             $('#play_button_' + ans.answer_key ).show();
            $('#play_button_' + ans.answer_key + ' > a').attr('video_id', ans.video_id);
            $('#play_button_' + ans.answer_key + ' > a').click(  function() { 
                var vid_id = $(this).attr('video_id');
                $('#video_link_' + vid_id).trigger('click');
            });
        }

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
function show_selector( data, results, id, text, selected ) {
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
         if( id.substr(0,4) == 'cc1_' ) {
              $('#province_selector_div').html('<p>{% trans %}By state or province:{% endtrans %}</p>');
              var cc1 = id.substr(4,2);

              show_province_selector(data, data.results['country'][cc1], 'all_provinces', '{% trans %}All{% endtrans %}', 1 );
              var province_id = 0;
              for (var province in data.results['province'][cc1]) {
                  province_id++;
                  show_province_selector(data, data.results['province'][cc1][province], 'province_' + province_id, province, 0 );
              }

         } else {
             // not a country, remove provinces from view
             $('#province_selector_div').html('');
         }
          show_data( data, results );
     } );
}


var selected_province = '';
function show_province_selector( data, results, id, text, selected ) {
     if( selected ) {
         selected_province = id;
         text_visible = 'visible';
         link_visible = 'hidden';
         selected_result = 'selected_result';
     } else  {
         text_visible = 'hidden';
         link_visible = 'visible';
         selected_result = '';
     }

     $('#province_selector_div').append( $("#res_selector_tmpl").tmpl( {
            'id' : id,
            'text' : text,
            'text_visible' : text_visible,
            'link_visible' : link_visible,
            'selected_result' : selected_result,
     }));

     $('#selector_link_' + id).click(function() {
          $("#selector_" + selected_province).removeClass('selected_result');
          $("#selector_link_" + selected_province).show();
          $("#selector_text_" + selected_province).hide();
          $("#selector_" + id).addClass('selected_result');
          $('#selector_link_' + id).hide();
          $('#selector_text_' + id).show();
          selected_province = id;

          show_data( data, results );
     } );
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

    if(data.video_id != 0) {
        $('#add_link_' + data.answer_key ).hide();
        $('#picture_' + data.answer_key ).html( $("#show_video_tmpl").tmpl({'answer_key': data.answer_key, 'video_id':data.video_id}));
        $('#play_button_' + data.answer_key ).show();
        $('#play_button_' + data.answer_key + ' > a').attr('video_id', data.video_id);
        $('#play_button_' + data.answer_key + ' > a').click(  function() { 
            var vid_id = $(this).attr('video_id');
            $('#video_link_' + vid_id).trigger('click');
        });
    }

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
                $('#answer_' + data.answer_key).parent().masonry({itemSelector: '.answer_card', isFitWidth: true
                });

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

function add_link_func( ans_key ) {
    $('#add_link_' + ans_key).hide();
    $('#link_form_div_' + ans_key).show();
    $('#answer_' + ans_key).parent().masonry({itemSelector: '.answer_card', isFitWidth: true
                        });

    $('#add_link_form_' + ans_key ).ajaxForm({
         dataType:  'json', 
         success: function(data) { 
             if( data.error ) {
                alert( data.error );
             } else {
                $('#link_form_div_' + ans_key).hide();
                $('#ans_txt_' + data.answer_key).html( '<a href="' + data.link + '">' + data.answer_text + '</a>' );
             }
         }
    });

//    alert('add link ' + ans_key );
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



window.fbAsyncInit = function() {

    FB.init({appId: '{{ facebook_app_id }}', status: true, cookie: true, xfbml: true});

    FB.Event.subscribe('{% if current_user %}auth.logout{% else %}auth.login{% endif %}', function(response) {
      {% if current_user %}
 
          window.location = "/logout"
 
      {% else %}

         {% if question is defined %}
             window.location = '{{BASE_URL}}/q{{question_key_name}}';
         {% else %}
             //alert('about to reload...');
             window.location = '{{BASE_URL}}';
         {% endif %}

      {% endif %}
    });


    {% if current_user %}
        $('#whole_page').show();

        $('#faces_div').html( $("#logged_in_faces_tmpl").tmpl({}) );
        FB.XFBML.parse( document.getElementById('faces_div') );

        {% if question is defined %}
            show_question_for_voting();

            // call here to immediately center
            $('#answers_div').masonry({
                 itemSelector: '.answer_card',
                 isFitWidth: true
            });

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
            {% if recent_questions_page %}
              set_recent_questions_width();
              $(window).on("debouncedresize", function( event ) {
                  set_recent_questions_width();
              });

            {% endif %}
        {% endif %}


        {% if not current_user.cc1 %}

            show_countries_modal();

        {% endif %}

    {% else %}

        FB.getLoginStatus(function(response) {
            if (response.status === 'connected') {
//                alert('connected, reload will happen, do not show content');
            } else {
//                alert('not authorized or not logged in, reload will NOT happen, show content, this was the solution to the ugly flicker when rendering the page...');
                $('#whole_page').show();

                {% if question is defined %}
                     show_question_for_welcome();
                     show_comments();

                     $('.answer_picture').imagesLoaded(
                     function(){ 
                          $('#answers_div').masonry({
                                           itemSelector: '.answer_card',
                                           isFitWidth: true
                           });
                     });
                {% else %}

                    $('#main_container').html( $("#welcome_msg_tmpl").tmpl({}));

                {% endif %}
//
// disabled until made nicer
//                FB.XFBML.parse( document.getElementById('welcome_msg') );
                FB.XFBML.parse( document.getElementById('second_login_button') );
            } 
        });

    {% endif %}
};

        (function(d){
           var js, id = 'facebook-jssdk'; if (d.getElementById(id)) {return;}
           js = d.createElement('script'); js.id = id; js.async = true;
           js.src = "//connect.facebook.net/en_US/all.js";
           d.getElementsByTagName('head')[0].appendChild(js);
         }(document));

$(function(){
    $('.fancybox-media').fancybox({
        openEffect  : 'none',
        closeEffect : 'none',
        helpers : {
            media : {}
        }
    });
});


$(window).load(function(){  
  $('#answers_div').masonry({
    itemSelector: '.answer_card',
    isFitWidth: true
  });
});

</script>

