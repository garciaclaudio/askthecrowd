<!-- JS TEMPLATES START -->

<script id="country_popup_tmpl" type="text/html">
<div class="country_popup_class">
    <p id="country_message">{% trans %}Select your country:{% endtrans %}</p>
    <p id="province_message" style="display:none;">{% trans %}Select your state:{% endtrans %}</p>
    <table style="border-spacing:5px;" id="country_popup">
    </table>
    <div id="other_ccs_link"><br /><a href="#" onclick="show_all_countries();return false;">{%trans%}other...{%endtrans%}</a></div>
    <div id="all_countries_select_box" style="display:none;">
        <select id="all_countries_select" onclick="select_country();return false;">
            <option>{%trans%}select{%endtrans%}</option>
        </select>
    </div>
    <table style="border-spacing:5px;" id="provinces_popup" style="display:none;">
    </table>
    <div id="back_ccs_link" style="display:none;"><br /><a href="#" onclick="show_countries_modal();return false;">{%trans%}back...{%endtrans%}</a></div>
</div>
</script>

<script id="country_popup_elem" type="text/html">
   <tr><td><a href="#" onclick="select_country('${cc1}');return false;"><img style="vertical-align:top" width="40" height="30" src="/images/flags/${cc1_lc}.png"><span>${name1}</span></a></td>
       <td><a href="#" onclick="select_country('${cc2}');return false;"><img style="vertical-align:top" width="40" height="30" src="/images/flags/${cc2_lc}.png">${name2}</a></td>
   </tr>
</script>

<script id="provinces_popup_elem" type="text/html">
   <tr><td><a href="#" onclick="select_province('${name1}');return false;">${name1}</a></td>
       <td><a href="#" onclick="select_province('${name2}');return false;">${name2}</a></td>
   </tr>
</script>


<script id="logged_in_faces_tmpl" type="text/html">
<fb:login-button show-faces="true"></fb:login-button>
</script>

<script id="welcome_msg_tmpl" type="text/html">
<div class="mytube">
<div id="welcome_msg"><p>{% trans %}To use this app, sign in with your facebook account:{% endtrans %}</p>
<fb:login-button show-faces="true"></fb:login-button>
</div>
</div>
</script>

<script id="show_pic_tmpl" type="text/html">
<img class="answer_picture" src="image?answer_key=${answer_key}">
</script>

<script id="show_video_tmpl" type="text/html">
<img class="answer_picture" width="192" src="http://img.youtube.com/vi/${video_id}/mqdefault.jpg">
<a class="fancybox-media invisible" id="video_link_${video_id}" href="http://www.youtube.com/watch?v=${video_id}">x</a>
</script>

<script id="show_answer_tmpl" type="text/html">
   <div class="answer_card" id="answer_${answer_key}">

    <div class="delete_answer_div">
        <form method="post" id="delete_answer_form_${answer_key}" action="/ajax.html">
            <input type="hidden" name="answer_key" value="${answer_key}">
            <input type="hidden" name="action" value="delete_answer">
<a href="#" onclick="$('#delete_answer_form_${answer_key}').submit();return false;"><b>x</b></a>
       </form>
   </div>

  <div class="card_inset">
        <div id="picture_${answer_key}"></div>
        <div id="play_button_${answer_key}" class="play_inset" style="display:none;"><a class="play_inset_link"><img src="/images/play_button_small.png"></a></div>
        <span class="answer_text" id="ans_txt_${answer_key}">${answer_text}</span>
        <div id="add_link_${answer_key}"><a href="#" class="video_link" onclick="add_link_func('${answer_key}');return false;">{% trans %}add link{% endtrans %}</a></div>

        <div id="link_form_div_${answer_key}" class="invisible">
             <form id="add_link_form_${answer_key}" action="/ajax.html" method="get">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="add_link">
                {% trans %}Link URL:{% endtrans %}<input id="link_link" name="link_link" class="text" type="text" size=20>
&nbsp;<button type="submit">{% trans %}add{% endtrans %}</button>
             </form>
        </div>

        <div id="file_uploader_${answer_key}"><noscript><p>Please enable JavaScript to use file uploader.</p><!-- or put a simple form for upload here --></noscript></div>
        <div id="add_video_link_${answer_key}" class="invisible"><a href="#" class="video_link" onclick="add_video_link_func('${answer_key}');return false;">{% trans %}add youtube video{% endtrans %}</a></div>
        <div id="video_form_div_${answer_key}" class="invisible">
             <form id="add_video_form_${answer_key}" action="/ajax.html" method="get">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="add_video">
                {% trans %}Youtube video URL:{% endtrans %}<input id="video_link" name="video_link" class="text" type="text" size=20>
&nbsp;<button type="submit">{% trans %}add{% endtrans %}</button>
             </form>
        </div>
  </div>

    </div> <!-- answer_card-->
</script>

<script id="question_created_tmpl" type="text/html">
<div id="question_created"> 
    <p>{% trans %}Question created!{% endtrans %}</p>
    <div class="results_card" style="max-width:60%;padding:15px;">
            <div style="font-size: 130%;margin-bottom:10px;">${question_text}</div>
            <div class="question_desc">${question_desc}</div>
    </div>
    <p>{% trans %}Suggest inital answers:{% endtrans %}</p>
    <div id="answers_form_div">
         <form id="new_answer_form" action="/ajax.html" method="get">
            <input type="hidden" name="question_key_name" value="${question_key_name}">
            <input type="hidden" name="action" value="add_answer">
            <textarea rows=2 id="new_answer" name="answer_text"></textarea>
            &nbsp;<button type="submit">{% trans %}add{% endtrans %}</button>
         </form>
    </div>

    <br />

    <div id="answers_div">
    </div>

    <div style="clear:both">
        <br /><button id="done_button" class="my_button">{% trans %}done{% endtrans %}</button>
        <input type="checkbox" name="post_to_fb" value="post_to_fb" checked>{% trans %}Post to Facebook{% endtrans %}<br>
    </div>
</div>
</script>

<script id="new_question_tmpl" type="text/html">
    <div id="question_form_div" class="results_card" style="width:350px;">
      <div style="margin:20px;">
        <form id="question_form" action="/ajax.html" method="get">
              <input type="hidden" name="action" value="create_question">

<div style="text-align:center;"><strong>{% trans %}Create a question{% endtrans %}</strong></div>

{{ "{{if show_uploader}" }}}
              <p><input type="file" name="upload_file"></p>
{{ "{{else}" }}}
              <p>{% trans %}Question:{% endtrans %}<input name="question" id="question" type="text" style="width:100%"></p>
              <p>{% trans %}Description (optional):{% endtrans %}
              <textarea name="question_desc" id="question_desc" rows=2 style="width:100%"></textarea>
              </p>
{{ "{{/if}" }}}
              <div style="text-align:center;">
                  <button type="submit" id="create_question_button" class="my_button">{% trans %}create{% endtrans %}</button>
             </div>

        </form>
    </div>
</div>
</script>

<script id="done_question_tmpl" type="text/html">
<div class="mytube">
    <p>{% trans %}You're done!{% endtrans %}</p>
    <div class="results_card" style="max-width:60%;padding:15px;">
            <div style="font-size: 130%;margin-bottom:10px;">${question_text}</div>
            <div class="question_desc">${question_desc}</div>
    </div>

    <p>{% trans %}Your question's URL is:{% endtrans %} <a target="_top" href="{{BASE_URL}}/q${question_key_name}">{{BASE_URL}}/q${question_key_name}</a></p>

    <button type="submit" id="create_another_button" class="my_button">{% trans %}create another question{% endtrans %}</button>
</div>
</script>


<script id="answer_question_tmpl" type="text/html">

    <div class="question_card" style="max-width:80%;padding-top:10px;">
            <div style="font-size: 130%;margin-bottom:10px;">${question_text}</div>
            <div class="question_desc">${question_desc}</div>

{{ "{{if playlist}" }}}
     <div class="vote_link">(<a class="fancybox-media" playlist="${playlist}" href="https://www.youtube.com/v/${first_video_id}?version=3&loop=1&autoplay=1">{% trans %}play all{% endtrans %}</a>)</div>
{{ "{{/if}" }}}

    </div>

    <div class="author_link" style="margin-bottom:10px;"><a href="{{BASE_URL}}/u${owner_id}" target="_top">{% trans %}by{% endtrans %} ${owner_name}</a></div>

    <div id="answers_div">
    </div>

<br />

    {% include "templates/comments.inc" %}

    <div class="vote_link add_ans_class add_ans_link">
      <div class="mytube">
        <a href="#" onclick="add_ans_link_func();return false;">{% trans %}suggest new answer{% endtrans %}</a>
      </div>
    </div>

    <div id="add_new_ans" class="add_ans_class" style="display:none;">
      <div class="mytube">
         <form id="add_new_ans_form" action="/ajax.html" method="get">
            <input type="hidden" name="question_key_name" value="${question_key_name}">
            <input type="hidden" name="action" value="add_answer">

<textarea rows=1 id="new_answer" name="answer_text"></textarea>

&nbsp;<button type="submit">{% trans %}add{% endtrans %}</button>
         </form>
      </div>
    </div>

    <div id="see_results_div" style="clear:both;display:none;padding-bottom:20px;">
        <button id="see_results_button" class="my_button">{% trans %}See results{% endtrans %}</button>

<input type="checkbox" name="post_to_fb" value="post_to_fb" checked>{% trans %}Post to Facebook{% endtrans %}<br>

    </div>

</div>
</script>

<script id="suggest_another_answer_tmpl" type="text/html">
    <p>{% trans %}Suggest another answer{% endtrans %}</p>

    <div id="answers_form_div">
         <form id="new_answer_form" action="/ajax.html" method="get">
            <input type="hidden" name="question_key_name" value="${question_key_name}">
            <input type="hidden" name="action" value="add_answer">

            <input id="new_answer" name="answer_text" class="text" type="text" size=30>
&nbsp;<button id="new_answer" type="submit">{% trans %}add{% endtrans %}</button>

         </form>
    </div>
</script>

</body>
</html>


<script id="show_answer_for_voting_tmpl" type="text/html">
  <div class="answer_card" id="answer_${answer_key}">

  <div class="card_inset">
        <div id="picture_${answer_key}"></div>
        <div id="play_button_${answer_key}" class="play_inset" style="display:none;"><a class="play_inset_link"><img src="/images/play_button_small.png"></a></div>

        <div class="answer_text">{{ "{{if link}" }}}<a href="${link}">${answer_text}</a>{{ "{{else}" }}}${answer_text}{{ "{{/if}" }}}</div>

    <div id="whirly_${answer_key}" class="count_inset" style="display:none;"><div class="whirly_box"><img src="/images/loading.gif"></div></div>

    <div id="count_inset_${answer_key}" class="count_inset"><div id="count_text_${answer_key}"  class="count_text"></div></div>


    {{ "{{html by_link}" }}}

    <div style="clear:both;">
        <div class="plus_vote_div">
            <form method="post" id="plus_vote_form_${answer_key}" action="/ajax.html">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="vote">
                <input type="hidden" name="vote_val" value="1">
                <span class="vote_link"><a href="#" onclick="$('#plus_vote_form_${answer_key}').submit();return false;">{% trans %}vote{% endtrans %}</a></span>
           </form>
       </div>

        <div id="minus_vote_link_${answer_key}" class="minus_vote_div">
            <form method="post" id="minus_vote_form_${answer_key}" action="/ajax.html">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="vote">
                <input type="hidden" name="vote_val" value="-1">
    <span class="vote_link"><a href="#" onclick="$('#minus_vote_form_${answer_key}').submit();return false;">{% trans %}unvote{% endtrans %}</a>
           </form>
       </div>
   </div>
<!--   <div class="author_link comment_div"><a href="#" answer_id="${answer_id}">{% trans %}comment{% endtrans %}</a></div><div><img style="width:15%;" src="http://fbcdn-profile-a.akamaihd.net/hprofile-ak-prn1/41650_518261219_2789_q.jpg"> <textarea name="comment_text" rows=2 style="width:80%"></textarea> -->

</div>

  </div>

  </div> <!-- answer_card-->
</script>



<script id="show_results_tmpl" type="text/html">
<div class="mytube">
<div> 
    <div class="question_card" style="max-width:80%;padding:15px;">
        <div style="font-size: 130%;margin-bottom:10px;">${question_text}</div>
        <div class="question_desc">${question_desc}</div>
{{ "{{if playlist}" }}}
     <div class="vote_link">(<a class="fancybox-media" playlist="${playlist}" href="https://www.youtube.com/v/${first_video_id}?version=3&loop=1&autoplay=1">{% trans %}play all{% endtrans %}</a>)</div>
{{ "{{/if}" }}}
    </div>
    <div class="author_link" style="margin-bottom:10px;"><a href="{{BASE_URL}}/u${owner_id}" target="_top">{% trans %}by{% endtrans %} ${owner_name}</a></div>

    <div id="selector_div" class="selector_class">
    </div>

    <div id="province_selector_div" class="selector_class">
    </div>

    <div id="result_answers_div">
    </div>
</div>    
</div>
</script>

<script id="res_selector_tmpl" type="text/html">
    <div id="selector_${id}" class="result_selector ${selected_result}"><span id="selector_text_${id}" class="${text_visible}">${text}</span><span id="selector_link_${id}" class="vote_link ${link_visible}"><a href="#" onclick="return false;">${text}</a></span></div>
</script>

<script id="show_answer_for_results_tmpl" type="text/html">
  <div class="results_card" id="answer_${answer_key}">
          <div class="results_count_text">${num_votes} &#10003;</div>
          <div class="results_pic" id="picture_${answer_key}"></div>
          <div id="play_button_${answer_key}" class="results_play_inset" style="display:none;"><a class="play_inset_link"><img src="/images/play_button_small.png"></a></div> 
          <div class="results_text">{{ "{{if link}" }}}<a href="${link}">${answer_text}</a>{{ "{{else}" }}}${answer_text}{{ "{{/if}" }}}</div>
  </div><br />
</script>


<script id="show_result_pic_tmpl" type="text/html">
<img class="answer_picture" src="image?answer_key=${answer_key}">
</script>

<script id="welcome_question_tmpl" type="text/html">
<div class="mytube">
  <div> 
<!--      <div id="welcome_msg" class="add_ans_class"><p>{% trans %}To vote on this question, sign in with your facebook account:{% endtrans %}</p> <fb:login-button show-faces="true"></fb:login-button> -->
      </div>
      <div class="question_text">${question_text}</div>
<br />
      <div id="answers_div">
      </div>

      <div id="second_login_button" class="add_ans_class" style="padding:15px;font-size:110%;clear:both;">{% trans %}To vote on this question, sign in with your facebook account:{% endtrans %}<br /><br /> <fb:login-button show-faces="true"></fb:login-button></div>
  </div>
</div>
</script>


<script id="show_answer_for_welcome_tmpl" type="text/html">
  <div class="answer_card" id="answer_${answer_key}">
    <div class="card_inset">
          <div id="picture_${answer_key}"></div>
          <div id="play_button_${answer_key}" class="play_inset" style="display:none;"><a class="play_inset_link"><img src="/images/play_button_small.png"></a></div>
          <span class="answer_text">{{ "{{if link}" }}}<a href="${link}">${answer_text}</a>{{ "{{else}" }}}${answer_text}{{ "{{/if}" }}}</span>
      <div id="count_inset_${answer_key}" class="count_inset"><div id="count_text_${answer_key}"  class="count_text"></div></div>
    </div>
  </div> <!-- answer_card-->
</script>


<script id="show_answer_for_voting_and_editing_tmpl" type="text/html">
  <div class="answer_card" id="answer_${answer_key}">

    <div class="delete_answer_div" id="del_ans_link_${answer_key}">
        <form method="post" id="delete_answer_form_${answer_key}" action="/ajax.html">
            <input type="hidden" name="answer_key" value="${answer_key}">
            <input type="hidden" name="action" value="delete_answer">
<a href="#" onclick="$('#delete_answer_form_${answer_key}').submit();return false;"><b>x</b></a>
       </form>
   </div>

   <div class="card_inset">
        <div id="picture_${answer_key}"></div>
        <div id="play_button_${answer_key}" class="play_inset" style="display:none;"><a class="play_inset_link"><img src="/images/play_button_small.png"></a></div>
        <span class="answer_text" id="ans_txt_${answer_key}">${answer_text}</span>
        <div id="add_link_${answer_key}"><a href="#" class="video_link" onclick="add_link_func('${answer_key}');return false;">{% trans %}add link{% endtrans %}</a></div>

        <div id="link_form_div_${answer_key}" class="invisible">
             <form id="add_link_form_${answer_key}" action="/ajax.html" method="get">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="add_link">
                {% trans %}Link URL:{% endtrans %}<input id="link_link" name="link_link" class="text" type="text" size=20>
&nbsp;<button type="submit">{% trans %}add{% endtrans %}</button>
             </form>
        </div>

        <div id="file_uploader_${answer_key}"><noscript><p>Please enable JavaScript to use file uploader.</p><!-- or put a simple form for upload here --></noscript></div>
        <div id="add_video_link_${answer_key}" class="invisible"><a href="#" class="video_link"  onclick="add_video_link_func('${answer_key}');return false;">{% trans %}add youtube video{% endtrans %}</a></div>
        <div id="video_form_div_${answer_key}" class="invisible">
             <form id="add_video_form_${answer_key}" action="/ajax.html" method="get">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="add_video">
                {% trans %}Youtube video URL:{% endtrans %}<input id="video_link" name="video_link" class="text" type="text" size=20>
&nbsp;<button type="submit">{% trans %}add{% endtrans %}</button>
             </form>
        </div>

    <div id="count_inset_${answer_key}" class="count_inset"><div id="count_text_${answer_key}"  class="count_text"></div></div>

    <div class="author_link"><a href="{{BASE_URL}}/u${owner_id}" target="_top">{% trans %}by{% endtrans %} ${owner_name}</a></div>

    <div style="clear:both;">
        <div class="plus_vote_div">
            <form method="post" id="plus_vote_form_${answer_key}" action="/ajax.html">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="vote">
                <input type="hidden" name="vote_val" value="1">
    <span class="vote_link"><a href="#" onclick="$('#plus_vote_form_${answer_key}').submit();return false;">{% trans %}vote{% endtrans %}</a>
           </form>
       </div>

        <div id="minus_vote_link_${answer_key}" class="minus_vote_div">
            <form method="post" id="minus_vote_form_${answer_key}" action="/ajax.html">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="vote">
                <input type="hidden" name="vote_val" value="-1">
    <span class="vote_link"><a href="#" onclick="$('#minus_vote_form_${answer_key}').submit();return false;">{% trans %}unvote{% endtrans %}</a>
           </form>
       </div>
   </div>


  </div>

  </div> <!-- answer_card-->
</script>


<script id="prev_questions_tmpl" type="text/html">
    <div class="prev_question_class" id="prev_question_${question_key_name}">
        <div class="mytube" style="float:left;">
             <div class="question_link"><a href="{{BASE_URL}}/q${question_key_name}" target="_top">${question_text}</a></div>
        </div>

        <div class="mytube" style="float:right;">
            <div id="more_link_${question_key_name}" q_key="${question_key_name}" class="vote_link"><a href="#" onclick="return false;">[{% trans %}open{% endtrans %}]</a></div>
            <div id="less_link_${question_key_name}" q_key="${question_key_name}" class="vote_link invisible"><a href="#" onclick="return false;">[{% trans %}close{% endtrans %}]</a></div>
        </div>

        <div class="mytube invisible" id="prev_answers_div_${question_key_name}" style="clear:both;">
             <div style="font-size:90%;color:#555555;font-style:italic;margin-bottom:5px;margin-left:5px;">${question_desc}</div>
             <div id="inner_prev_answers_div_${question_key_name}"></div>
             <div style="clear:both;text-align:center;"><button class="my_button delete_question_button" q_key="${question_key_name}">{% trans %}delete{% endtrans %}</button></div>
       </div>
   </div>
</script>

<script id="prev_questions_header_tmpl" type="text/html">

    <div id="dialog-confirm" class="invisible" title="{% trans %}Delete this question?{% endtrans %}">
	<p><span class="ui-icon ui-icon-alert" style="float:left; margin:0 7px 20px 0;"></span>
        <span id="confirm_delete_question_div"></span>
</p>
    </div>

</script>



<script id="show_answer_for_prev_questions_tmpl" type="text/html">
  <div class="answer_card" id="answer_${answer_key}">

    <div class="delete_answer_div" id="del_ans_link_${answer_key}">
        <form method="post" id="delete_answer_form_${answer_key}" action="/ajax.html">
            <input type="hidden" name="answer_key" value="${answer_key}">
            <input type="hidden" name="action" value="delete_answer">
<a href="#" onclick="$('#delete_answer_form_${answer_key}').submit();return false;"><b>x</b></a>
       </form>
   </div>

    <div id="count_inset_${answer_key}" class="prev_questions_count_inset ${visible}"><div id="count_text_${answer_key}" class="count_text">${num_votes}&#10003;</div></div>

   <div class="card_inset">
        <div id="picture_${answer_key}"></div>
          <div id="play_button_${answer_key}" class="play_inset" style="display:none;">
              <a class="play_inset_link"><img src="/images/play_button_small.png"></a>
          </div>
        <span class="answer_text" id="ans_txt_${answer_key}">{{ "{{if link}" }}}<a href="${link}">${answer_text}</a>{{ "{{else}" }}}${answer_text}{{ "{{/if}" }}}</span>

    {{ "{{if link}" }}}{{ "{{else}" }}}
        <div id="add_link_${answer_key}"><a href="#" class="video_link" onclick="add_link_func('${answer_key}');return false;">{% trans %}add link{% endtrans %}</a></div>


        <div id="link_form_div_${answer_key}" class="invisible">
             <form id="add_link_form_${answer_key}" action="/ajax.html" method="get">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="add_link">
                {% trans %}Link URL:{% endtrans %}<input id="link_link" name="link_link" class="text" type="text" size=20>
&nbsp;<button type="submit">{% trans %}add{% endtrans %}</button>
             </form>
        </div>
    {{ "{{/if}" }}}

        <div id="file_uploader_${answer_key}"><noscript><p>Please enable JavaScript to use file uploader.</p><!-- or put a simple form for upload here --></noscript></div>


        <div id="add_video_link_${answer_key}" class="invisible"><a href="#" class="video_link"  onclick="add_video_link_func('${answer_key}');return false;">{% trans %}add youtube video{% endtrans %}</a></div>

        <div id="video_form_div_${answer_key}" class="invisible">
             <form id="add_video_form_${answer_key}" action="/ajax.html" method="get">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="add_video">
                {% trans %}Youtube video URL:{% endtrans %}<input id="video_link" name="video_link" class="text" type="text" size=20>
&nbsp;<button type="submit">{% trans %}add{% endtrans %}</button>
             </form>
        </div>
   </div>

  </div> <!-- answer_card-->
</script>


<script id="all_questions_question_tmpl" type="text/html">
    <div class="prev_question_class" id="prev_question_${question_key_name}">
        <div class="mytube" style="float:left;">
             <div class="question_link"><a target="_top" href="{{BASE_URL}}/q${question_key_name}">${question_text}</a></div>
        </div>
    </div>
</script>


<script id="show_comment_tmpl" type="text/html">
   <div id="comment_${comment_key}">
    <div class="results_card comments_card" style="width:450px;">
{{ "{{if by_user}" }}}
    <div class="delete_answer_div"><a class="delete_comment_link" onclick="return false;" comment_key=${comment_key} href="#"><b>x</b></a></div>
{{ "{{/if}" }}}
        <div class="comments_text">
            <a href="#">${owner_name}:</a> ${comment_text}
        </div>
    </div>
  </div>
</script>



