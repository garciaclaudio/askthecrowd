
var answers = {};

function show_question_for_voting() {

     show_comments();

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
             'question_desc': "{{question.question_desc}}",
             'owner_name': "{{owner_name}}",
             'owner_id': "{{owner_id}}",
             'question_key_name': "{{question_key_name}}",
	     }));

    $('#new_answer').autosize();
             
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

            var pic_link = logo_pic_link
            if( selected_ans_votes > 0 ) {
                pic_link = images_base_url + '/image?answer_key=' + selected_answer_key
            }

//alert('pic link: ' + pic_link );

            name = "{{question.question_text}}";
            caption = "{{user_name}} " + ' {% trans %}answered this question on Five Votes.{% endtrans %}';

{% if  user_is_male %}
            var desc = '{% trans %}His top choice was:{% endtrans %} ' + selected_answer_text;
{% else %}
            var desc = '{% trans %}Her top choice was:{% endtrans %} ' + selected_answer_text;
{% endif %}

            var obj = {
              method: 'feed',
              link: '{{BASE_URL}}' + '/q{{question_key_name}}',
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

      var by_link = '';

      {% if ans.show_owner %}
          by_link = '<div class="author_link"><a href="{{BASE_URL}}/u{{ ans.owner_id }}">{% trans %}by{% endtrans %} {{ ans.owner_name }}</a></div>';
      {% endif %}

      $('#answers_div').append( $("#show_answer_for_voting_tmpl").tmpl( {
         'by_link' : by_link,
         'answer_key' : '{{ans.answer_key}}',
         'answer_text' : '{{ans.answer_text}}',
         'link' : '{{ans.link}}',
      }));

      {% if ans.has_pic %}
          $('#picture_' + '{{ans.answer_key}}').html( $("#show_pic_tmpl").tmpl({'answer_key': '{{ans.answer_key}}'} ));
      {% endif %}

      {% if ans.video_id != 'None' %}
          $('#picture_' + '{{ans.answer_key}}' ).html( $("#show_video_tmpl").tmpl({'answer_key': '{{ans.answer_key}}', 'video_id':'{{ans.video_id}}' }));
          $('#play_button_' + '{{ans.answer_key}}' ).show();
          $('#play_button_' + '{{ans.answer_key}}' + ' > a').attr('video_id', '{{ans.video_id}}');
          $('#play_button_' + '{{ans.answer_key}}' + ' > a').click(  function() { 
              var vid_id = $(this).attr('video_id');
              $('#video_link_' + vid_id).trigger('click');
          });
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
        $('#see_results_div')[0].scrollIntoView(true);
    {% endif %}
    // add and bind new_answer_form here
}


function show_question_for_welcome() {

    $('#main_container').html( $("#welcome_question_tmpl").tmpl({
             'question_text': "{{question.question_text}}",
             'question_desc': "{{question.question_desc}}",
             'question_key_name': "{{question_key_name}}",
	     }));

    {% for ans in answers %}
      $('#answers_div').append( $("#show_answer_for_welcome_tmpl").tmpl( {
         'answer_key' : '{{ans.answer_key}}',
         'link' : '{{ans.link}}',
         'answer_text' : '{{ans.answer_text}}',
      }));

      {% if ans.has_pic %}
          $('#picture_' + '{{ans.answer_key}}').html( $("#show_pic_tmpl").tmpl({'answer_key': '{{ans.answer_key}}'} ));
      {% endif %}
    {% endfor %}
}


function show_results( data ) {
    $('#main_container').fadeOut('fast');

    $('#main_container').html( $("#show_results_tmpl").tmpl({
            'owner_id' : data.owner_id,
            'owner_name' : data.owner_name,
            'question_text': data.question_text,
            'question_desc': data.question_desc,
            'question_key_name': data.question_key_name,
          }));

    show_selector(data, data.results['all'], 'all', '{% trans %}All{% endtrans %}', 1 );
    show_selector(data, data.results['female'], 'female', '{% trans %}Women{% endtrans %}', 0 );
    show_selector(data, data.results['male'], 'male', '{% trans %}Men{% endtrans %}', 0 );

//    for (var i = 0; i < 20; i++) {
//        show_selector(data, data.results['male'], 'male_'+i , '{% trans %}Men{% endtrans %}', 0 );
//    }

    for (var cc1 in data.results['country']) {
       show_selector(data, data.results['country'][cc1], 'cc1_' + cc1, data.patronimics[cc1], 0 );
    }


    for (var i = 0; i < data.friends_with_votes.length; i++) {
        friend_id = 'friend_' + data.friends_with_votes[i].user_id;
        show_selector(data, data.results[friend_id], friend_id, data.friends_with_votes[i].name, 0 );
    }

    show_data( data, data.results['all'] );

// foreach all, women, men, friend list
//   add and bind a result_selector link
//   "all" is selected by default
//   have a global var keeping track of what's selected
//   when something new is selected:
//      (hide link, show span) of new, unselect previous (hide span, show link)

    $('#main_container').fadeIn('fast');
}


function add_comment_func() {
    $('#add_comment').show();
    $('#comment_link').hide();
}


function _set_comments_width() {

    var parent_width = $('#main_container').width();
    if( parent_width > 400 ) {
       $('.comments_card').width(450);
    } else {
       $('.comments_card').width(parent_width-40);
    }
}


function show_comments() {

    _set_comments_width();
    $(window).on("debouncedresize", function( event ) {
        _set_comments_width();
    });

    $('#comment_link').show();

    // bind "comment" click
    $('#comment_form').ajaxForm({
         dataType:  'json', 
         success: function(data) { 
             if( data.error ) {
                 alert( data.error );
             } else {
                //clean comment form
                $('#comment_box').val('');
                alert('add comment to page');
             }
         }
    });


// call this from show_question_for_voting / show_question_for_welcome
//
// load comments after page is done, with an ajax call 
//
// if there are comments, add them using template calls, and show the comment form, hide the link
//
// (if user owns the comment, show delete link)
//
// if there are no comments, show the link, hide the comment form
//
// (if called from welcome view, do not allow form submission)

}

