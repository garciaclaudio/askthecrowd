
<div id="comments_div">

    <div class="results_card" style="width: 450px;"> 
        <div class="results_text">
A comment
        </div>  
    </div>  

    <div class="results_card" style="width:450px;"> 
        <div class="results_text">
A comment
        </div>  
    </div>  

</div>


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

<div id="comment_link" class="vote_link add_ans_class">
      <div class="mytube">
        <a href="#" onclick="add_comment_func();return false;">{% trans %}comment{% endtrans %}</a>
      </div>
</div>

<br />
