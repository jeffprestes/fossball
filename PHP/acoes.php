<?php
require './util.php';

$acao = $_GET["acao"];

if ($acao == "novapartida")     {
    if (!temPartidaAberta() && !temPartidaEmAndamento())     {
        $sql = "insert into partidas (cod_status, data_inicio) values (1, NOW())";
        if (executaStatement($sql))     {
            header("Location: partida.php");
        }
    }
}

?>
