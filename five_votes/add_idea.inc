{% load i18n %}
<div id="thanks_for_adding_div" style="display:none">
  <h1>{% trans "Thanks for sending!" %}</h1>

  <h2>(show preview here, that they can accept / edit further ALSO validate that title is not a duplicate)</h2>

{% trans "Your idea is now available for others to rate and comment on." %}

<p>
<a id="add_another_idea_link">{% trans "add another idea" %}</a>
</p>

</div>

    <form id="add_idea_form" action="ajax.html" method="post"> 
<br />
        <input type="hidden" name="action" value="save_idea" />

        <input type="hidden" name="match_key" value="{{ match_key }}" />

        <input id="fb_uid_input" type="hidden" name="fb_uid" value="{{ user.fb_id }}" />

                <label>{% trans "Title" %}</label><br \>
                <input class="span-9-FOO" type=text name="idea_title" size=40 maxlength=128 value="{{idea.title }}">
<br />
<br />

<label>{% trans "Description" %}</label>
<br />
                <textarea class="span-13" style="height:40px;" name="idea_desc" id="idea_desc">{{ idea.desc }}</textarea>
<br />
<div class="quiet" style="clear:both;">{% trans "A short paragraph explaining the idea." %}</div>
<br />
<label>{% trans "Extended description (optional)" %}</label>
<br />
                <textarea class="span-13" style="height:120px;" name="idea_extended_desc" id="idea_extended_desc">{{ idea.extended_desc }}</textarea>
<br />
<div class="quiet" style="clear:both;">{% trans "Bold and italic text in the displayed letter can be obtained by surrounding it with <strong>strong</strong> and <em>em</em> html tags. Example: &lt;strong&gt;Joe&lt;/strong&gt; will be shown as <strong>Joe</strong>." %}</div>
<br />

                <p><input type="submit" value="{% trans "Save" %}" /></p>
<br />
        </form>

