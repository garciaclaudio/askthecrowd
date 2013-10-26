
<div id="main_container">
 <div id="result_answers_div">

  <div class="recent_question_card" id="answer_FOO">
      <div style="float:left;margin-right:10px;">
          <img src="http://graph.facebook.com/518261219/picture?type=square">
      </div>
      <div class="rec_q_link"><a href="/q1234" target="_top">Foo bar bar doo?</a> (ver _set_results_width)</div>
      <div style="margin-top:10px;">
          <div style="float:left;margin-right:10px;">
              <img src="/images/flags/mx.png">
          </div>
          <div class="author_link" style="margin-bottom:10px;"><a href="u1234" target="_top">Claudio García <br/>Nuevo León, México</a></div>
     </div>
  </div>


<br />

  <div class="results_card" id="answer_FOO">
      <div class="results_count_text">FOO BAR &#10003;</div>
      <div class="results_pic" id="picture_FOO">

<img class="answer_picture" width="100" height="100" src="/image?answer_key=ag5kZXZ-Zml2ZS12b3Rlc3INCxIGQW5zd2VyGOMBDA">

      </div>
      <div class="results_text">Foo bar, foo bar boo boo hoo doo?</div>
  </div>

 </div>
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
