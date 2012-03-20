{% load i18n %}

<div id="fb-root"></div>

<script>

function show_answer(data) {
    $('#answers_div').append( $("#show_answer_tmpl").tmpl( {
       'answer_key': data.answer_key,
       'answer_text': data.answer_text,
    }));

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
//            sizeLimit: 100000,
            uploadText: '{% trans "add picture" %}',
            allowedExtensions: ['jpg', 'jpeg'],
            onComplete: function(id, file_name, data){
                $('#file_uploader_' + data.answer_key).remove();
                $('#picture_' + data.answer_key).html( $("#show_pic_tmpl").tmpl({'answer_key': data.answer_key} ));

//alert( id + '--' + file_name + ' -- ' + data.answer_key)

            },
            params: {
                      action: 'upload_picture',
		      answer_key : data.answer_key,
            }
    });


}

function show_created_question(data) {

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
        $('#main_container').fadeOut('fast');
        $('#main_container').html( $("#done_question_tmpl").tmpl({
             'question_text': data.question_text,
             'question_key_name': data.question_key_name,
           }));
        $('#main_container').fadeIn('fast');
    });

    $('#main_container').fadeIn('fast');
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
}


function update_votes_header( votes_left ) {
    if( votes_left == 0 ) {
       $('#app_logo').html('{% trans "Zero Votes" %}');
    }
    else if( votes_left == 1 ) {
       $('#app_logo').html('{% trans "One Vote" %}');
    }
    else if( votes_left == 2 ) {
       $('#app_logo').html('{% trans "Two Votes" %}');
    }
    else if( votes_left == 3 ) {
       $('#app_logo').html('{% trans "Three Votes" %}');
    }
     else if( votes_left == 4 ) {
       $('#app_logo').html('{% trans "Four Votes" %}');
    }
     else if( votes_left == 5 ) {
       $('#app_logo').html('{% trans "Five Votes" %}');
    }
}

function show_votes( answer_key, num_votes ) {
    // change color of answer card
    $("#answer_" + answer_key).addClass('voted_answer');
    // show / update value of count_inset div
    $("#count_inset_" + answer_key).show();
    $("#count_text_" + answer_key).html( num_votes + '&#10003;' );
    // make unvote link appear
    $("#minus_vote_link_" + answer_key).show();
    // update votes left counter (the header div)
}


function show_results() {

        $('#main_container').fadeOut('fast');
// AQUI VOY
//
//        $('#main_container').html( $("#show_results_tmpl").tmpl({
//             'question_text': data.question_text,
//             'question_key_name': data.question_key_name,
//           }));
        $('#main_container').fadeIn('fast');
}


function show_question_for_voting() {

    $('#main_container').html( $("#answer_question_tmpl").tmpl({
             'question_text': "{{question.question_text}}",
             'question_key_name': "{{question_key_name}}",
	     }));

    $('#see_results_button').click( function() {
        show_results();
    });

    {% for ans in answers %}

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

      // bind vote buttons here
     $('#plus_vote_form_' + '{{ans.answer_key}}' ).ajaxForm({
         dataType:  'json', 
         success: function(data) { 
             if( data.error ) {
                alert( data.error );
             } else {
                if( data.votes_left <= 0 ) {
                   $('.plus_vote_div').hide();
                   $('#see_results_div').show();
                }
                show_votes( data.answer_key, data.new_count );
                update_votes_header( data.votes_left );
             }
         }
     });

     $('#minus_vote_form_' + '{{ans.answer_key}}' ).ajaxForm({
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
                    $("#count_inset_" + data.answer_key).hide();
                    $("#answer_" + data.answer_key).removeClass('voted_answer');
                    $("#minus_vote_link_" + data.answer_key).hide();
                } else {
                    $("#count_text_" + data.answer_key).html( data.new_count + '&#10003;' );
                }
             }
         }
     });

// alert( 'answer: ' + '{{ ans.answer_text }}' );

    {% endfor %}

    update_votes_header( {{votes_left}} );

    {% if votes_left <= 0 %}
        $('.plus_vote_div').hide();
        $('#see_results_div').show();
    {% endif %}

    // add and bind new_answer_form here
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
          FB.Event.subscribe('auth.login', function(response) {
              // Reload the entire page. Could also do an Ajax request and dynamically
              // update the already loaded page.
              window.location.reload(true);
          });
//
//          FB.ui({ method: 'feed', 
//                  message: 'Facebook for Websites is super-cool'});

            FB.getLoginStatus(function(response) {

              if (response.status === 'connected') {
                // the user is logged in and connected to your
                // app, and response.authResponse supplies
                // the user's ID, a valid access token, a signed
                // request, and the time the access token 
                // and signed request each expire
                var uid = response.authResponse.userID;
                var accessToken = response.authResponse.accessToken;

                show_new_question();
            
//            alert('user connected ' +  uid + ' -- ' + accessToken );

// XXX poner codigo en un div, llamar FB.XFBML.Host.parseDomTree();
// (ver index.html)
//                   $('#login_div').html(
//                      "<p><fb:profile-pic uid='loggedinuser' facebook-logo='true'></fb:profile-pic> Welcome, <fb:name uid='loggedinuser' useyou='false'></fb:name>. You are logged in with your Facebook account.</p>" );
//                   FB.XFBML.parse( document.getElementById('login_div') );

              } else if (response.status === 'not_authorized') {
                // the user is logged in to Facebook, 
                //but not connected to the app

                $('#welcome_msg').show();

//            alert( 'not authorized' );
            
              } else {
                // the user isn't even logged in to Facebook.
            
            alert( 'not logged in to FB' );
            
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


