  <div id="footer">
    <div class="mytube">

      <div class="vote_link" id="all_users_questions" style="float:left;margin-top:8px;margin-left:15px;font-size:0.6em;">

     {% if question %}
        <a href="{{BASE_URL}}/add" target="_top">{% trans %}Create and share your own questions!{% endtrans %}</a>
     {% else %}
        <a href="{{BASE_URL}}/all" target="_top">{% trans %}Questions from other users{% endtrans %}</a>
     {% endif %}

      </div>


    <div style="float:right;margin-top:4px;margin-bottom:10px;">
        <form method="get">
        <select name="lang" onChange="submit()">
              <option value="en" {% if language_en %}selected="selected"{% endif %}>English</option>
              <option value="es" {% if language_es %}selected="selected"{% endif %}>Español</option>
        </select>
        </form>
    </div>

    <div style="float:right;margin-top:8px;">{% trans %}Select your language{% endtrans %}:&nbsp;</div>
    </div>
  </div>
