{% load i18n %}

  <div id="footer">
    <div class="mytube">

      <div class="vote_link" id="all_users_questions" style="float:left;margin-top:8px;margin-left:15px;font-size:0.6em;">

     {% if question %}
        <a href="{{BASE_URL}}" target="_top">{% trans "Create and share your own questions!" %}</a>
     {% else %}
        <a href="{{BASE_URL}}/all" target="_top">{% trans "Questions from other users" %}</a>
     {% endif %}

      </div>


    <div style="float:right;margin-top:5px;margin-bottom:10px;">
        <form method="get">
        <select name="lang" onChange="submit()">
              <option value="en" {% if language_en %}selected="selected"{% endif %}>{% lang2name "en" %}</option>
              <option value="es" {% if language_es %}selected="selected"{% endif %}>{% lang2name "es" %}</option>
        </select>
        </form>
    </div>
    <div style="float:right;margin-top:9px;">{% trans "Select your language" %}:&nbsp;</div>
    </div>
  </div>
