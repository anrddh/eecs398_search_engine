function search_query() {
  var url = $("#query").val();
  // window.open(url);
  window.location.href = url;
}

$(document).ready(function(){
    $('#query').keypress(function(e){
      if(e.keyCode==13)
        $('#search').click();
    });
});