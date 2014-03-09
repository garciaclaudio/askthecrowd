
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


function add_comment_func() {
    $('#add_comment').show();
    $('#comment_link').hide();
}
