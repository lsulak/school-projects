<?
$tituly = mysqli_query($db, "SELECT * FROM titul ORDER BY cas DESC LIMIT 0, 7");
?>
<ul>
    <?
    while ($row = mysqli_fetch_array($tituly)){
    echo "<li>";
    echo "<h3><div id='datum_pridania'> [" . substr($row["cas"], 8, 2) . "/" . substr($row["cas"], 5, 2) . "/" . substr($row["cas"], 0, 4) . "]" . "</div>";
    $t_id = $row["id"];
    $nazov = $row["nazev"];
    ?>
    <div id='autori'><a class='nazov' href='index.php?pg=titul&id=<? echo $t_id; ?>'>
            <? echo $nazov . "</a>";
            $autor_titul = mysqli_query($db, "SELECT id_autor FROM autor_titul WHERE id_titul='$t_id'");
            while ($row_at = mysqli_fetch_array($autor_titul)) {
                $a_id = $row_at["id_autor"];
                $autor = mysqli_query($db, "SELECT * FROM autor WHERE id='$a_id'");
                $row_a = mysqli_fetch_assoc($autor);
                echo ", "  ?> <br><a class='autor_col'
                                     href='index.php?pg=tituly&id=<? echo $a_id; ?>'><? echo $row_a["prijmeni"] . ", " . $row_a["jmeno"] . " "; ?> </a> <?
            }
            echo "</div>";
            echo "</li>";
            }
            ?>
</ul>