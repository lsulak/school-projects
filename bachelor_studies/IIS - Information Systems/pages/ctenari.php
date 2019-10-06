<?php
if ($_SESSION['role'] < 2)
    echo "Stranka dostupná pouze zaměstnancům.";
else {
    ?>

    <h2>Prehled čtenářů</h2>
    <div class="caption">Čtenáři</div>
    <div id="table">
        <div class="header-row row">
            <span class="cell primary">Jmeno</span>
            <span class="cell primary">Login</span>
            <span class="cell primary">Adresa</span>
            <span class="cell primary">Telefon</span>
            <span class="cell primary">Email</span>
        </div>
        <?    $ctenari = mysqli_query($db, "SELECT * FROM ctenar");
        while ($row = mysqli_fetch_array($ctenari)) {
            ;?>
            <div class="row">
						<span class="cell primary" data-label="jmeno"><?
                            $c_id = $row["id"];
                            ?><a href='index.php?pg=ctenar&id=<? echo $c_id; ?>'>
                                <? echo $row["prijmeni"] . ", " . $row["jmeno"] . "</a>"; ?>
						</span>

						<span class="cell" data-label="login"><?
                            echo $row["login"]; ?></span>

						<span class="cell" data-label="adresa"><?
                            echo $row["adresa"]; ?></span>

						<span class="cell" data-label="telefon"><?
                            echo $row["telefon"]; ?></span>

						<span class="cell" data-label="email">
							<a href="mailto:<? echo $row["email"]; ?>">
                                <? echo $row["email"]; ?> </a>
						</span>

            </div>

        <? } ?>
    </div>
<? } ?>
