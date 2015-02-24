<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="content-type" content="text/html;charset=utf-8"/>
    
    <!-- Set the viewport to show the page at a scale of 1.0, and make it non-scalable -->
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=0"/>
    
    <!-- Make it fullscreen / hide the browser URL bar -->
    <meta name="apple-mobile-web-app-capable" content="yes" />
    <meta name="mobile-web-app-capable" content="yes">
    
    <!--
        Give the status bar another colour
        Valid values for "content" are: "default" (white), "black" and "black-translucent"
        If set to "default" or "black", the content is displayed below the status bar. If set to
        "black-translucent", the content is displayed under the bar.
    -->
    <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent" />
    
    <!--
        Add a Home icon.
        By default, Mobile Safari searches for a file named "apple-touch-icon.png" in the root directory of your website.
        If it can't find any image there, you can specify it using the code below. Make sure the image has a dimension
        of 114x114 and is a PNG file. The glossy finish and resizing for the different devices will be done automatically.
        
        In case you don't want the gloss applied, use "apple-touch-icon-precomposed" instead of "apple-touch-icon".
    -->
    <link rel="apple-touch-icon img_src" href="images/apple-touch-icon.png" />
    
    <!--
        Add a splash screen / startup image.
        Take note this file exactly needs to be 320x460 for iPhone or 1004x768 for iPad, and is a PNG file.
        Also, this only works if "apple-mobile-web-app-capable" is set to "yes".
    -->
    <link rel="apple-touch-startup-image" href="images/apple-splash.png" />
    
    <link href='http://fonts.googleapis.com/css?family=Droid+Sans' rel='stylesheet' type='text/css'>
    
    <!--
        Prevent the user from elastic scrolling / rubber banding. Sadly, doesn't always work properly.
        For a more solid solution, check out ScrollFix ( https://github.com/joelambert/ScrollFix )
    -->
    <script>
    function BlockElasticScroll(event) {
        event.preventDefault() ;
    }
    
    function mudaPagina(destino)    {
        var pagDestino = '';
        
        document.getElementById('aguarde').style.display = 'block';
        document.getElementById('divNewBie').style.display = 'none';
        document.getElementById('divVeterano').style.display = 'none';
        document.getElementById('divPlacar').style.display = 'none';
        
        if (destino == 'newbie')    {
            pagDestino = 'newplayer.php';
        }   else if (destino=='veterano') {
            pagDestino = 'checkplayer.php';
        }   else {
            pagDestino = 'placar.php';
        }
        
        document.location.href=pagDestino;
    }
    
    
    
    </script>

    <title>Pebolim Social</title>
    <link rel="stylesheet" type="text/css" href="css/style.css"/>

</head>
<body ontouchmove="BlockElasticScroll(event);">
    <div id="divSplash"><img id="imgSplash" src="images/imagem-topo.jpg"></div>
    <div id="divNewBie" onclick="mudaPagina('newbie')"><img id="imgNewBie" src="images/football16.png"><div id="txtNewBie"><span>Novo jogador</span></div></div>
    <div id="divVeterano" onclick="mudaPagina('veterano')"><img id="imgVeterano" src="images/football45.png"><div id="txtVeterano"><span>Veterano</span></div></div>
    <div id="divPlacar" onclick="mudaPagina('placar')"><img id="imgPlacar" src="images/football141.png"><div id="txtPlaca"><span>Placar</span></div></div>
    <div id="aguarde"><img src="images/ajax-loader.gif"><br /><span>Aguarde</span></div>
</body>
</html>
