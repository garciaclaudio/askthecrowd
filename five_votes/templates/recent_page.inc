
<div id="main_container">
    <div id="result_answers_div">

  <div class="results_card" id="answer_FOO">
      <div class="results_count_text">FOO BAR &#10003;</div>
      <div class="results_pic" id="picture_FOO">

<img class="answer_picture" width="100" height="100" src="/image?answer_key=ag5kZXZ-Zml2ZS12b3Rlc3INCxIGQW5zd2VyGOMBDA">

      </div>
      <div class="results_text">Foo bar, foo bar boo boo hoo doofoo bar boo boo hoo doofoo bar boo boo hoo doofoo bar boo boo hoo doo?</div>
  </div><br />

  <div class="results_card" id="answer_FOO">
      <div class="results_count_text">FOO BAR &#10003;</div>
      <div class="results_pic" id="picture_FOO">

<img class="answer_picture" width="100" height="100" src="/image?answer_key=ag5kZXZ-Zml2ZS12b3Rlc3INCxIGQW5zd2VyGOMBDA">

      </div>
      <div class="results_text">Foo bar, foo bar boo boo hoo doo?</div>
  </div><br />

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
