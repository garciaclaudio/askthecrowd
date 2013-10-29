
<div id="main_container">

    <h4>{% trans %}Recent questions{% endtrans %}</h4>

 <div id="result_answers_div">

{% for q in questions %}
   <div class="recent_question_card">
      <div style="float:left;margin-right:10px;">
          <img src="http://graph.facebook.com/518261219/picture?type=square">
      </div>
      <div class="rec_q_link"><a href="{{BASE_URL}}/q{{q.question_key_name}}" target="_top">{{q.question_text}}</a></div>
      <div class="question_desc">{{q.question_desc}}</div>
      <div style="margin-top:10px;">
          <div style="float:left;margin-right:10px;">
              <img src="/images/flags/{{q.cc1}}.png">
          </div>
          <div class="author_link" style="margin-bottom:10px;"><a href="u1234" target="_top">{{q.owner_name}} <br/>{{q.province}}, {{q.cc_name}}</a></div>
     </div>
   </div>
   <br />
{% endfor %}

 </div> <!-- result_answers_div -->
</div> <!-- main container -->


