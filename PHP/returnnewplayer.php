<?php
session_start();

require './util.php';

//echo $_SESSION["token"];

$dado = getUserData($sandbox, $_SESSION["token"]);
 
//Obtem o primeiro resultado do retorno do getUser
$dado = $dado[0];


if (registraNovoUsuario($dado))     {
    $_SESSION["userData"] = $dado;
    header("Location: partida.php");
}
?>
