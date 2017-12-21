<!doctype html>
<html lang="es">
  <head>
    <title>Arbre dels Desitjos - Ripolab.org</title>
    <!-- Required meta tags -->
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    
    <!-- Latest compiled and minified CSS -->
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous">

    <!-- Optional theme -->
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css" integrity="sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp" crossorigin="anonymous">
    
    <link href="https://fonts.googleapis.com/css?family=Share+Tech+Mono" rel="stylesheet"> 
    
    <link rel="stylesheet" href="style.css">
    
    <!-- https://www.cssscript.com/canvas-based-html5-hsv-color-picker-component/ -->
    <script src="html5kellycolorpicker.min.js"></script>
    
  </head>
  <body id="thankyou">
    <div id="wrapper">
        <a href="https://www.ripolab.org"><img class="img-responsive" src="ripolab.png"></a>
        <?php
            include 'config.php'; 

            $conn = new mysqli($servername, $username, $password, $dbname);
            // Check connection
            if ($conn->connect_error) {
                die("Connection failed: " . $conn->connect_error);
            }

            $sql = 'INSERT INTO deseos (color)
            VALUES ("'.$_POST['color'].'")';

            if ($conn->query($sql) === TRUE) {
                echo "<h3>El desig nº " . $conn->insert_id . " s'ha enviat correctament</h3>";
            } else {
                echo "Error: " . $sql . "<br>" . $conn->error;
            }

            $conn->close();
        ?> 
        <div><a href="http://nadal.ripolab.org"><button class="btn btn-default btn-lg">Enviar un altre desig</button></a></div>
        <div id="difusion">
            <p>Si t'ha agradat el nostre "Arbre dels Desitjos", comparteix-ho amb tothom. Agrairem la teva difusió.</p>
        </div>
        <!-- https://ayudawp.com/anadir-enlaces-para-compartir-en-redes-sociales-sin-plugins/ -->
        <div class="share-post">
            <h4>Comparteix</h4>
        </div> <!-- /. share-post -->
        <a href="https://www.facebook.com/sharer/sharer.php?u=http://navidad.ripolab.org" class="facebook" target="_blank">
            Facebook
        </a>
        <a href="https://twitter.com/intent/tweet?url=http://navidad.ripolab.org&text=Arbre%20dels%20Desitjor%20per%20@Ripolab" class="twitter" target="_blank">
            Twitter
        </a>
        <a href="https://plus.google.com/share?url=http://navidad.ripolab.org" class="googleplus" target="_blank">
            Google+
        </a>
    </div>
</body> 