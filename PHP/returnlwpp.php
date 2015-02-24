<?php
session_start();

$_SESSION["userData"] = $_GET;

//print_r($_SESSION["userData"]);
header("Location: partida.php");
?>
