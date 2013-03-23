{% load i18n %}
<!-- JS TEMPLATES START -->

<script id="logged_in_faces_tmpl">
<fb:login-button show-faces="true"></fb:login-button>
</script>

<script id="welcome_msg_tmpl">
<div class="mytube">
<div id="welcome_msg"><p>{% trans "To use this app, sign in with your facebook account:" %}</p>
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
        <div id="play_button_${answer_key}" class="play_inset" style="display:none;"><a class="play_inset_link"><img src="/images/play_button_small.png"></a></div>        <span class="answer_text">${answer_text}</span>
        <div id="file_uploader_${answer_key}"><noscript><p>Please enable JavaScript to use file uploader.</p><!-- or put a simple form for upload here --></noscript></div>
        <div id="add_video_link_${answer_key}" class="invisible"><a href="#" onclick="add_video_link_func('${answer_key}');return false;">{% trans "add youtube video" %}</a></div>
        <div id="video_form_div_${answer_key}" class="invisible">
             <form id="add_video_form_${answer_key}" action="/ajax.html" method="get">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="add_video">
                {% trans "Youtube video URL:" %}<input id="video_link" name="video_link" class="text" type="text" size=20>
&nbsp;<button type="submit">{% trans "add" %}</button>
             </form>
        </div>

  </div>

    </div> <!-- answer_card-->
</script>

<script id="question_created_tmpl" type="text/html">
<div class="mytube">
<div id="question_created"> 
    <p>{% trans "Question created" %}</p>
    <div class="question_text">${question_text}</div>

    <p>{% trans "Suggest inital answers:" %}</p>
    <div id="answers_form_div">
         <form id="new_answer_form" action="/ajax.html" method="get">
            <input type="hidden" name="question_key_name" value="${question_key_name}">
            <input type="hidden" name="action" value="add_answer">

            <input id="new_answer" name="answer_text" class="text" type="text" size=30>
&nbsp;<button id="new_answer" type="submit">{% trans "add" %}</button>

         </form>
    </div>

    <br />

    <div id="answers_div">
    </div>

    <div style="clear:both">
        <br /><button id="done_button" class="my_button">{% trans "done" %}</button>
        <input type="checkbox" name="post_to_fb" value="post_to_fb" checked>{% trans "Post to Facebook" %}<br>
    </div>
</div>
</div>
</script>

<script id="new_question_tmpl" type="text/html">
<div class="mytube">
    <div id="question_form_div">
        <form id="question_form" action="/ajax.html" method="get">
              <input type="hidden" name="action" value="create_question">
              <label for="question">{% trans "Enter your question:" %}</label><br /><br />
<div>              <input name="question" id="question" size=45 class="text" type="text">
</div>
<br />
            <button type="submit" id="create_question_button" class="my_button">{% trans "create" %}</button>
        </form>
    </div>
</div>
</script>

<script id="done_question_tmpl" type="text/html">
<div class="mytube">
    <p>{% trans "You're done!" %}</p>
    <div class="question_text">${question_text}</div>

    <p>{% trans "Your question's URL is:" %} <a target="_top" href="{{BASE_URL}}/q${question_key_name}">{{BASE_URL}}/q${question_key_name}</a></p>

    <button type="submit" id="create_another_button" class="my_button">{% trans "create another question" %}</button>
</div>
</script>


<script id="answer_question_tmpl" type="text/html">

    <div class="question_text">${question_text}
    </div>

    <div class="author_link" style="margin-bottom:10px;"><a href="{{BASE_URL}}/u${owner_id}" target="_top">{% trans "by" %} ${owner_name}</a></div>

    <div id="answers_div">
    </div>

<br />

    <div class="vote_link add_ans_class add_ans_link">
      <div class="mytube">
        <a href="#" onclick="add_ans_link_func();return false;">{% trans "suggest new answer" %}</a>
      </div>
    </div>

    <div id="add_new_ans" class="add_ans_class" style="display:none;">
      <div class="mytube">
         <form id="add_new_ans_form" action="/ajax.html" method="get">
            <input type="hidden" name="question_key_name" value="${question_key_name}">
            <input type="hidden" name="action" value="add_answer">

            <input id="new_answer" name="answer_text" class="text" type="text" size=20>
&nbsp;<button id="new_answer" type="submit">{% trans "add" %}</button>
         </form>
      </div>
    </div>

<br />


    <div id="see_results_div" style="clear:both;display:none;padding-bottom:20px;">
        <button id="see_results_button" class="my_button">{% trans "See results" %}</button>

<input type="checkbox" name="post_to_fb" value="post_to_fb" checked>{% trans "Post to Facebook" %}<br>

    </div>

</div>
</script>

<script id="suggest_another_answer_tmpl" type="text/html">
    <p>{% trans "Suggest another answer" %}</p>

    <div id="answers_form_div">
         <form id="new_answer_form" action="/ajax.html" method="get">
            <input type="hidden" name="question_key_name" value="${question_key_name}">
            <input type="hidden" name="action" value="add_answer">

            <input id="new_answer" name="answer_text" class="text" type="text" size=30>
&nbsp;<button id="new_answer" type="submit">{% trans "add" %}</button>

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

        <span class="answer_text">${answer_text}</span>


    <div id="whirly_${answer_key}" class="count_inset" style="display:none;"><img src="/images/loading.gif"></div>
    <div id="count_inset_${answer_key}" class="count_inset"><div id="count_text_${answer_key}"  class="count_text"></div></div>

    <div class="plus_vote_div">
        <form method="post" id="plus_vote_form_${answer_key}" action="/ajax.html">
            <input type="hidden" name="answer_key" value="${answer_key}">
            <input type="hidden" name="action" value="vote">
            <input type="hidden" name="vote_val" value="1">
<span class="vote_link"><a href="#" onclick="$('#plus_vote_form_${answer_key}').submit();return false;">{% trans "vote" %}</a>
       </form>
   </div>

    <div id="minus_vote_link_${answer_key}" class="minus_vote_div">
        <form method="post" id="minus_vote_form_${answer_key}" action="/ajax.html">
            <input type="hidden" name="answer_key" value="${answer_key}">
            <input type="hidden" name="action" value="vote">
            <input type="hidden" name="vote_val" value="-1">
<span class="vote_link"><a href="#" onclick="$('#minus_vote_form_${answer_key}').submit();return false;">{% trans "unvote" %}</a>
       </form>
   </div>

{% templatetag openbrace %}{% templatetag openbrace %}html by_link{% templatetag closebrace %}{% templatetag closebrace %}

  </div>

  </div> <!-- answer_card-->
</script>



<script id="show_results_tmpl" type="text/html">
<div class="mytube">
<div> 
    <div class="question_text">${question_text}</div>

    <div class="author_link" style="margin-bottom:10px;"><a href="{{BASE_URL}}/u${owner_id}" target="_top">{% trans "by" %} ${owner_name}</a></div>

    <div id="selector_div">
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
      <div class="results_text">${answer_text}</div>
  </div><br />
</script>


<script id="show_result_pic_tmpl" type="text/html">
<img class="answer_picture" width="100" height="100" src="image?answer_key=${answer_key}">
</script>


<script id="welcome_question_tmpl" type="text/html">
<div class="mytube">
  <div> 
      <div id="welcome_msg"><p>{% trans "To vote on this question, sign in with your facebook account:" %}</p> <fb:login-button show-faces="true"></fb:login-button>
      </div>
      <div class="question_text">${question_text}</div>
<br />
      <div id="answers_div">
      </div>

      <div id="second_login_button" class="add_ans_class" style="padding:15px;font-size:110%;clear:both;">{%trans "login to vote" %} <br /><br /> <fb:login-button show-faces="true"></fb:login-button></div>
  </div>
</div>
</script>


<script id="show_answer_for_welcome_tmpl" type="text/html">
  <div class="answer_card" id="answer_${answer_key}">
    <div class="card_inset">
          <div id="picture_${answer_key}"></div>
          <div id="play_button_${answer_key}" class="play_inset" style="display:none;"><a class="play_inset_link"><img src="/images/play_button_small.png"></a></div>
          <span class="answer_text">${answer_text}</span>
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
        <span class="answer_text">${answer_text}</span>
        <div id="file_uploader_${answer_key}"><noscript><p>Please enable JavaScript to use file uploader.</p><!-- or put a simple form for upload here --></noscript></div>
        <div id="add_video_link_${answer_key}" class="invisible"><a href="#" onclick="add_video_link_func('${answer_key}');return false;">{% trans "add youtube video" %}</a></div>
        <div id="video_form_div_${answer_key}" class="invisible">
             <form id="add_video_form_${answer_key}" action="/ajax.html" method="get">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="add_video">
                {% trans "Youtube video URL:" %}<input id="video_link" name="video_link" class="text" type="text" size=20>
&nbsp;<button type="submit">{% trans "add" %}</button>
             </form>
        </div>
    <div id="count_inset_${answer_key}" class="count_inset"><div id="count_text_${answer_key}"  class="count_text"></div></div>

    <div class="plus_vote_div">
        <form method="post" id="plus_vote_form_${answer_key}" action="/ajax.html">
            <input type="hidden" name="answer_key" value="${answer_key}">
            <input type="hidden" name="action" value="vote">
            <input type="hidden" name="vote_val" value="1">
<span class="vote_link"><a href="#" onclick="$('#plus_vote_form_${answer_key}').submit();return false;">{% trans "vote" %}</a>
       </form>
   </div>

    <div id="minus_vote_link_${answer_key}" class="minus_vote_div">
        <form method="post" id="minus_vote_form_${answer_key}" action="/ajax.html">
            <input type="hidden" name="answer_key" value="${answer_key}">
            <input type="hidden" name="action" value="vote">
            <input type="hidden" name="vote_val" value="-1">
<span class="vote_link"><a href="#" onclick="$('#minus_vote_form_${answer_key}').submit();return false;">{% trans "unvote" %}</a>
       </form>
   </div>

    <div class="author_link"><a href="{{BASE_URL}}/u${owner_id}" target="_top">{% trans "by" %} ${owner_name}</a></div>

  </div>

  </div> <!-- answer_card-->
</script>


<script id="prev_questions_tmpl" type="text/html">
    <div class="prev_question_class" id="prev_question_${question_key_name}">
        <div class="mytube" style="float:left;">
             <div class="question_link"><a href="{{BASE_URL}}/q${question_key_name}" target="_top">${question_text}</a></div>
        </div>

        <div class="mytube" style="float:right;">
            <div id="more_link_${question_key_name}" q_key="${question_key_name}" class="vote_link"><a href="#" onclick="return false;">[{% trans "open" %}]</a></div>
            <div id="less_link_${question_key_name}" q_key="${question_key_name}" class="vote_link invisible"><a href="#" onclick="return false;">[{% trans "close" %}]</a></div>
        </div>

        <div class="mytube invisible" id="prev_answers_div_${question_key_name}" style="clear:both;">
             <div id="inner_prev_answers_div_${question_key_name}"></div>
             <div style="clear:both;text-align:center;"><button class="my_button delete_question_button" q_key="${question_key_name}">{% trans "delete" %}</button></div>
       </div>
   </div>
</script>

<script id="prev_questions_header_tmpl" type="text/html">

    <div id="dialog-confirm" class="invisible" title="{% trans "Delete this question?" %}">
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
        <span class="answer_text">${answer_text}</span>
        <div id="file_uploader_${answer_key}"><noscript><p>Please enable JavaScript to use file uploader.</p><!-- or put a simple form for upload here --></noscript></div>
        <div id="add_video_link_${answer_key}" class="invisible"><a href="#" onclick="add_video_link_func('${answer_key}');return false;">{% trans "add youtube video" %}</a></div>
        <div id="video_form_div_${answer_key}" class="invisible">
             <form id="add_video_form_${answer_key}" action="/ajax.html" method="get">
                <input type="hidden" name="answer_key" value="${answer_key}">
                <input type="hidden" name="action" value="add_video">
                {% trans "Youtube video URL:" %}<input id="video_link" name="video_link" class="text" type="text" size=20>
&nbsp;<button type="submit">{% trans "add" %}</button>
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

