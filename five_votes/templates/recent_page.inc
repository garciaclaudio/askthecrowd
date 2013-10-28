
<div id="main_container">
 <div id="result_answers_div">

{% for q in questions %}
   <div class="recent_question_card" id="answer_FOO">
      <div style="float:left;margin-right:10px;">
          <img src="http://graph.facebook.com/518261219/picture?type=square">
      </div>
      <div class="rec_q_link"><a href="{{BASE_URL}}/q{{q.question_key_name}}" target="_top">{{q.question_text}}</a></div>
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




<div id="recent_page_div">

<h4>{% trans %}Recent questions{% endtrans %}</h4>

      {% for q in questions %}
           <div class="question_link usr_page_link"><a href="{{BASE_URL}}/q{{q.question_key_name}}" target="_top">{{q.question_text}}</a></div>
           <p>{{q.owner_name}}</p>
           <p>{{q.cc_name}}</p>
           <p>{{q.cc1}}</p>
           <p>{{q.province}}</p>
      {% endfor %}

</div> <!-- usr_page -->
