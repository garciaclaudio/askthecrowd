{% load i18n %}

<script type="text/javascript">

$(function(){

    $('#question_form').ajaxForm({
         dataType:  'json', 
         success: function(data) { 
             if( data.error ) {
                 alert( data.error );
             } else {
//	         $("#add_idea_form").hide();
//                 $("#thanks_for_adding_div").show();
             }
         }
     });

    $('#create_question_button').click(
          $('#question_form').submit();
    );


});

</script>


</script>
