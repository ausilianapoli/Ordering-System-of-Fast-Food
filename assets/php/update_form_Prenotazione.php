<?php
	include_once 'dbh.php';
?>


<!DOCTYPE html>
<html lang=ita>
<head><meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" type="text/css" href="../css/style.css">
	<title>La mia Panineria</title>
</head>
<body>
	<h2>Modifica una prenotazione</h2>
	<p>Inserisci una delle prenotazioni presenti nel sistema: </p>
	<?php
		$sql = "SELECT * FROM prenotazione;";
		$result = mysqli_query($conn, $sql);
		$resultCheck = mysqli_num_rows($result);

		if ($resultCheck > 0) {
			while($row = mysqli_fetch_assoc($result)){
				echo "(".$row['ID_Prenotazione'].", ".$row['ID_Tavolo'].", ".$row['Data_Ora'].", ".$row['tot_panini'].", ".$row['tot_bevande'].", ".$row['totale_EUR'].")<br>";
			}
		}
	?>
	<p></p>
	<form action="update_Prenotazione.php" method="POST">
		<input type="number" name="ID_Prenotazione_old" placeholder="ID prenotazione vecchio - Es. 1">
		<input type="text" name="ID_Tavolo_new" placeholder="ID tavolo nuovo - Es. T02">
		<button type="submit" name="submit">Modifica</button>
	</form>
	<p></p>
	<div class="vertical-menu">
		<a href="../../index.php" class=active>Torna alla Home</a>
	</div>
</body>
</html>