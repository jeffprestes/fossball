<?php

$sandbox = true;


/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

function getToken($sandbox = true)
{
    //Endpoint da API
    $apiEndpoint  = 'https://www.novatrix.com.br/gateway/gettoken.php';
  
    $data = array('projeto' => 'Pimbolim');
    $data_string = json_encode($data);
    
    return connectGateway($sandbox, $apiEndpoint, $data_string);
    
}


function getUserData($sandbox = true, $token)    {
    
    //Endpoint da API
    $apiEndpoint  = 'https://www.novatrix.com.br/gateway/getuser.php';
  
    $data = array('token' => $token);
    $data_string = json_encode($data);
    
    return connectGateway($sandbox, $apiEndpoint, $data_string, true);
}


function connectGateway($sandbox = true, $endPoint, $dataToPost, $retornoArray = false)    
{
    //Endpoint da API
    $apiEndpoint  = $endPoint;
  
    //Executando a operação
    $curl = curl_init();
    
    
    $headers = array('Accept: application/json', 'Content-Type: application/json', 'Content-Length: ' . strlen($dataToPost));
    if (!$sandbox)  {
        array_push($headers, "Environment: Live");
    }
  
    curl_setopt($curl, CURLOPT_URL, $apiEndpoint);
    curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($curl, CURLOPT_POST, true);
    curl_setopt($curl, CURLOPT_POSTFIELDS, $dataToPost);
    curl_setopt($curl, CURLOPT_HTTPHEADER, $headers);
  
    $response = urldecode(curl_exec($curl));
    
    curl_close($curl);
  
    //Tratando a resposta
    //return $response;
    if ($retornoArray)      {
        return json_decode($response, $retornoArray);
    } else {
        return json_decode($response);
    }
    
}

function registraNovoUsuario($userData)     {
    $sql = "insert into jogadores (token, email, primeiro_nome, sobrenome) values (";
    $sql .= "'" . $userData['token'] . "', ";
    $sql .= "'" . $userData['email'] . "', ";
    $sql .= "'" . $userData['firstname'] . "', ";
    $sql .= "'" . $userData['lastname'] . "') ";
    
    //echo $sql . "\r\n";
    //exit;
    return executaStatement($sql);
}

function getBanco()     {
    $conexao = mysqli_connect("mysql10.novatrix.com.br", "novatrix11", "Jeff2308", "novatrix11");
    mysqli_select_db($conexao,"novatrix11");
    return $conexao;
}

function executaStatement($sql)     {
    $conexao = getBanco();
    $retorno = mysqli_query($conexao, $sql);
    mysqli_close($conexao);
    return $retorno;
}

function temPartidaAberta()     {
    $link = getBanco();
    
    $sql = "select cod_partida from partidas where cod_status = 1";
    $result = mysqli_query($link, $sql);
    $numLinhas = mysqli_num_rows($result);
    mysqli_close($link);
    
    if ($numLinhas>0)    {
        return true;
    }   else    {
        return false;
    }
}


function temPartidaEmAndamento()     {
    $link = getBanco();
    
    $sql = "select cod_partida from partidas where cod_status = 2";
    $result = mysqli_query($link, $sql);
    $numLinhas = mysqli_num_rows($result);
    mysqli_close($link);
    
    if ($numLinhas>0)    {
        return true;
    }   else    {
        return false;
    }
}

/*
 * 
 * 
 * 
 * 
 */
