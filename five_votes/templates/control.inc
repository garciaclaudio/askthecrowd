{% load i18n %}

<script type="text/javascript">

$(function(){

    $('#question_form').ajaxForm({
         dataType:  'json', 
         success: function(data) { 
             if( data.error ) {
                 alert( data.error );
             } else {

alert(data.question_key);

// show question with "question created" message
// show link "suggest answers"
//    clicking on it, makes suggest answer form appear
//       form is just a text field and a button
//       submitting creates the answer, gets it displayed 
//       (with an X for deleting), and shows link "add another"
//           shown answer includes "add picture" link
//               clicking on it, makes upload pic form appear
//                   submitting creates pic, gets it displayed 
// show "Done" button
//     clicking makes the new question appear as the "most recent" question
//     in the question list below
//
//	         $("#add_idea_form").hide();
//                 $("#thanks_for_adding_div").show();
             }
         }
     });

});

</script>


</script>

