{% load i18n %}

<div id="main_container">
</div> <!-- main container -->

<div class="add_ans_class">
          <div style="float:left;padding:10px;"><div class="fb-comments" data-href="{{BASE_URL}}/q{{question_key_name}}" data-num-posts="4" data-width="400"></div></div>
          <div style="float:left;margin-left:10px;padding:10px;" id="faces_div"></div>
</div>
<br />

<div class="vote_link add_ans_class add_ans_link">
    <div class="mytube">
       <a href="{{BASE_URL}}" target="_top">{% trans "Create your own questions!" %}</a>
    </div>
</div>
