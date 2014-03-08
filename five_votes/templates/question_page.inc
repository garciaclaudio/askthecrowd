<div id="main_container">
</div> <!-- main container -->

<br />


<div id="add_comment" class="add_ans_class" style="display:none;">
      <div class="mytube">
         <form id="comment_form" action="/ajax.html" method="get">
            <input type="hidden" name="question_key_name" value="${question_key_name}">
            <input type="hidden" name="action" value="add_comment">

<textarea rows=1 id="new_answer" name="comment_text"></textarea>

&nbsp;<button type="submit">{% trans %}comment{% endtrans %}</button>
         </form>
      </div>
</div>

<div class="vote_link add_ans_class">
      <div class="mytube">
        <a href="#" onclick="add_comment_func();return false;">{% trans %}comment{% endtrans %}</a>
      </div>
</div>


<br />

<div class="vote_link add_ans_class" style="margin-bottom:20px;">
    <div class="mytube">
       <a href="{{BASE_URL}}/add" target="_top">{% trans %}Create your own questions!{% endtrans %}</a>
    </div>
</div>
