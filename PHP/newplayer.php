<?php
session_start();

require './util.php';

$retorno = getToken();

//echo $retorno;

$_SESSION["token"] = $retorno->token;

header("Location: " . $retorno->url);
?>
