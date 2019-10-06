<? if ($_SESSION['role'] > 0) { ?>
    <div>
        <? $u_id = $_SESSION['logged'];
        if ($_GET['what'] == 1) {
            ?>
            <div class="caption">Vámi rezervované tituly</div>
            <div id="table">
                <div class="header-row row">
                    <span class="cell primary">Název</span>
                    <span class="cell">Autor</span>
                    <span class="cell">Rok vydáni</span>
                    <span class="cell">Status</span>
                    <span class="cell">Odrezervovat</span>
                </div>
                <?
                $rezervace = mysqli_query($db, "SELECT id, id_titul, stav FROM rezervace WHERE id_ctenar='$u_id'");
                while ($row = mysqli_fetch_array($rezervace)) {
                    $t_id = $row["id_titul"];
                    $titul = mysqli_query($db, "SELECT id, nazev, rok_vydani FROM titul WHERE id='$t_id'");
                    $row_t = mysqli_fetch_assoc($titul);
                    $autor_titul = mysqli_query($db, "SELECT id_autor FROM autor_titul WHERE id_titul='$t_id'");
                    ?>
                    <div class="row">
                        <span class="cell primary" data-label="nazev"><? echo $row_t["nazev"]; ?></span>
					<span class="cell" data-label="autor">
						<? while ($row_at = mysqli_fetch_array($autor_titul)) {
                            $a_id = $row_at["id_autor"];
                            $autor = mysqli_query($db, "SELECT * FROM autor WHERE id='$a_id'");
                            $row_a = mysqli_fetch_assoc($autor) ?>
                            <a title="Zobrazit knihy autora" href='index.php?pg=tituly&id=<? echo $row_a["id"]; ?>'>
                                <? echo $row_a["prijmeni"] . ", " . $row_a["jmeno"] ?></a>
                        <? } ?>
						</span>
                        <span class='cell' data-label='rok_vydani'> <? echo $row_t["rok_vydani"] ?> </span>
						<span class='cell' data-label='status'> 
							<?
                            if ($row['stav'] == 0)
                                echo "Zatím nepotvrzeno";
                            else echo "Potvrzeno";
                            ?>
						</span>
                        <span class='cell' data-label='odregistrovat'><a
                                href='_unreserve.php?id=<? echo $row["id"]; ?>'>(Odrezervovat)</a></span>
                    </div>
                    <? ;
                } ?>
            </div>
            <? ;
        } ?>
    </div>
<?
} else {
    echo "Tato stránka je jen pro příhlášené";
}?>