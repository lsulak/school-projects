<?

$u_id = $_SESSION['logged'];
$rezer = mysqli_num_rows(mysqli_query($db, "SELECT id FROM rezervace WHERE id_ctenar='$u_id' AND id_titul='$t_id'"));
if ($rezer == 0) {
    $pocet = mysqli_num_rows(mysqli_query($db, "SELECT id FROM vytisk WHERE id_titul='$t_id'"));
    $dostu = mysqli_num_rows(mysqli_query($db, "SELECT id FROM vytisk WHERE id_titul='$t_id' AND dostupnost='1'"));
    if ($dostu == 0 && $pocet == 0)
        echo "<b>Připravujeme</b>";
    else {
        echo $dostu . " z " . $pocet . " výtisků volných";
    }
    if ($dostu != 0 && $_SESSION['role'] == 1) {
        ?> <a href='_reserve.php?id=<? echo $t_id; ?>'>(Rezervovat)</a> <?
    }
} //rezer
else
    echo "<b>Rezervovano</b>";
?>