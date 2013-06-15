var modal = (function(){
    var 
    method = {},
    $overlay,
    $modal,
    $content,
    $close;

    // Center the modal in the viewport
    method.center = function () {
     var top, left;

     top = Math.max($(window).height() - $modal.outerHeight(), 0) / 2;
     left = Math.max($(window).width() - $modal.outerWidth(), 0) / 2;

     $modal.css({
      top:top + $(window).scrollTop(), 
      left:left + $(window).scrollLeft()
     });
    };

    // Open the modal
    method.open = function (settings) {
     $content.empty().append(settings.content);

     $modal.css({
      width: settings.width || 'auto', 
      height: settings.height || 'auto'
     });

     method.center();
     $(window).bind('resize.modal', method.center);
     $modal.show();
     $overlay.show();
    };

    // Open the modal
    method.create = function (settings) {
     $content.empty().append(settings.content);

     $modal.css({
      width: settings.width || 'auto', 
      height: settings.height || 'auto'
     });

     method.center();
     $(window).bind('resize.modal', method.center);
    };

    method.display = function () { 
     method.center();
     $modal.show();
     $overlay.show();
    };

    // Close the modal
    method.close = function () {
     $modal.hide();
     $overlay.hide();
     $content.empty();
     $(window).unbind('resize.modal');
    };

    // Generate the HTML and add it to the document
    $overlay = $('<div id="modal_overlay"></div>');
    $modal = $('<div id="modal"></div>');
    $content = $('<div id="modal_content"></div>');
    $close = $('<a id="modal_close" href="#">close</a>');

    $modal.hide();
    $overlay.hide();
    $modal.append($content, $close);

    $(document).ready(function(){
     $('body').append($overlay, $modal);      
    });

    $close.click(function(e){
     e.preventDefault();
     method.close();
    });

    return method;
}());


function show_countries_modal() {

    modal.create({content: $("#country_popup_tmpl").tmpl({}) });

    $.getJSON("ajax.html?action=get_countries",
        function(data){
            var targetCCs = [ "AR", "MX", "VE", "US" ];
            var content='';
            for (var i=0; i<targetCCs.length; ++i) {
                var cc1 = targetCCs[i];
                $('#country_popup').append($("#country_popup_elem").tmpl( {
                      'cc1': cc1,
                      'name': data['countries'][cc1],
                }));
            }
            modal.display();


//            alert( 'AF? ' + data['countries']['AF']  );
//
//            for (var i = 0; i < data['divisions']['MX'].length; i++) {
//                alert('estado? ' + data['divisions']['MX'][i]);
//            }

        }
    );
}


function select_country(cc1) {
    alert( 'CC: ' + cc1 );
}
