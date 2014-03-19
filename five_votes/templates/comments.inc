
<div id="comments_div">
    <div class="results_card comments_card" style="width:450px;">
        <div class="delete_answer_div"><a class="delete_comment_link" href="#"><b>x</b></a></div>

        <div class="comments_text">
            <a href="#">Claudio Garcia:</a> A comment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment. (date here in gray, not now...)
        </div>
    </div>
<br/>
    <div class="results_card comments_card" style="width:450px;">
        <div class="delete_answer_div"><a class="delete_comment_link" href="#"><b>x</b></a></div>

        <div class="comments_text">
            <a href="#">Claudio Garcia:</a> A comment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment, a commment. (date here in gray, not now...)
        </div>
    </div>
</div>

<div id="add_comment" class="results_card comments_card" style="width:450px;display:none;margin-bottom:20px;"> 
         <form id="comment_form" action="/ajax.html" method="get">
            <input type="hidden" name="question_key_name" value="${question_key_name}">
            <input type="hidden" name="action" value="add_comment">

<textarea rows=2 id="comment_box" name="comment_text" style="width:97%"></textarea>
<input type="checkbox" name="post_to_fb" value="post_to_fb" checked>{% trans %}Share on Facebook{% endtrans %}<br/>
<button type="submit">{% trans %}comment{% endtrans %}</button>
         </form>
</div>  

<div id="comment_link" class="vote_link add_ans_class">
      <div class="mytube">
        <a href="#" onclick="add_comment_func();return false;">{% trans %}comment{% endtrans %}</a>
      </div>
</div>

<br />
