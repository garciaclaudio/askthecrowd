<div id="recent_page_div">

    {% if num_questions %}
<h4>{% trans %}Recent questions{% endtrans %}</h4>
      {% for q in questions %}
           <div class="question_link usr_page_link"><a href="{{BASE_URL}}/q{{q.question_key_name}}" target="_top">{{q.question_text}}</a></div>
           <p>{{q.owner_name}}</p>
           <p>{{q.cc_name}}</p>
           <p>{{q.cc1}}</p>
           <p>{{q.province}}</p>
      {% endfor %}
    {% endif %}

</div> <!-- usr_page -->
