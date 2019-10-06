<?php
/*automaticky skript pro zmenu stavu pujcky*/
include "mysql.php";
$vp = mysqli_query($db, "SELECT * FROM vypujcka");
while($row=mysqli_fetch_array($vp)){
    $change = 0;
    $v_id = $row['id'];
    $vrac= $row['datum_vraceni'];
    if(strlen($vrac)==7){
        $vrac = "0".$vrac;
    }
    $vden = substr($vrac, 0, 2);
    $vmes = substr($vrac, 2, 2);
    $vrok = substr($vrac, 4);
    $aden = date("d");
    $ames = date("m");
    $arok = date("Y");
    if($arok > $vrok) //2015 > 2014
        $change = 1;
    else {
        if($ames > $vmes) //prosinec 14> listopad 14;
            $change = 1;
        else {
            if($aden > $vden) //11. prosince > 10. prosince
                $change = 1;
        }
    }
    if($change==1){
        mysqli_query($db, "UPDATE vypujcka SET stav='2' WHERE id='$v_id'");
    }
}
?>