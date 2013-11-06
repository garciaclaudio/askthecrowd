<div id="usr_page_div">

<img src="http://graph.facebook.com/{{user.id}}/picture?type=square"><BR />
<fb:login-button autologoutlink="true" scope="publish_stream"></fb:login-button>

    {% if num_questions %}
<h4>{% trans %}Questions by{% endtrans %} {{user.name}}</h4>
      {% for q in questions %}
           <div class="question_link usr_page_link"><a href="{{BASE_URL}}/q{{q.question_key_name}}" target="_top">{{q.question_text}}</a></div>
      {% endfor %}
    {% endif %}

    {% if num_ans_questions %}
<h4>{% trans %}Questions with answers by{% endtrans %} {{user.name}}</h4>
      {% for q in ans_questions %}
           <div class="question_link usr_page_link"><a href="{{BASE_URL}}/q{{q.question_key_name}}" target="_top">{{q.question_text}}</a></div>
      {% endfor %}
    {% endif %}

    {% if num_voted_questions %}
<h4>{% trans %}Questions voted by{% endtrans %} {{user.name}}</h4>
      {% for q in voted_questions %}
           <div class="question_link usr_page_link"><a href="{{BASE_URL}}/q{{q.question_key_name}}" target="_top">{{q.question_text}}</a></div>
      {% endfor %}
    {% endif %}

</div> <!-- usr_page -->
