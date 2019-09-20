<?php
	include_once 'dbh.php';
	header('Content-type: text/html; charset=UTF-8');
	mysqli_set_charset($conn, "utf8");
?>
<!DOCTYPE html>
<html lang=ita>
<head><meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" type="text/css" href="../css/style.css">
	<title>La mia Panineria</title>
</head>
<body>
	<h2>Modifica un ordine di panini</h2>
	<p>Inserisci uno degli ordini di panini presenti nel sistema: </p>
	<?php
		$sql = "SELECT * FROM ordine_pan;";
		$result = mysqli_query($conn, $sql);
		$resultCheck = mysqli_num_rows($result);

		if ($resultCheck > 0) {
			while($row = mysqli_fetch_assoc($result)){
				echo "(".$row['ID_Prenotazione'].", ".$row['ID_Panino'].", ".$row['Quantità'].", ".$row['note'].")<br>";
			}
		}
	?>
	<p></p>
	<form action="update_Ordine_Pan.php" method="POST">
		<input type="number" name="ID_Prenotazione_old" placeholder="ID prenotazione vecchio - Es. 1">
		<input type="text" name="ID_Panino_new" placeholder="ID panino nuovo - Es. T02">
		<input type="number" name="quantita_new" placeholder="Quantita' nuova - Es. 3">
		<input type="text" name="note_new" placeholder="Note nuove - Es. più pomodoro">
		<button type="submit" name="submit">Modifica</button>
	</form>
	<p></p>
	<div class="vertical-menu">
		<a href="../../index.php" class=active>Torna alla Home</a>
	</div>
</body>
</html>