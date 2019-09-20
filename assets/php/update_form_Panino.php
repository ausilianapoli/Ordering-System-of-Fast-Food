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
	<h2>Modifica un panino</h2>
	<p>Inserisci uno dei panini presenti nel sistema: </p>
	<?php
		$sql = "SELECT * FROM panino;";
		$result = mysqli_query($conn, $sql);
		$resultCheck = mysqli_num_rows($result);

		if ($resultCheck > 0) {
			while($row = mysqli_fetch_assoc($result)){
				echo "(".$row['ID_Panino'].", ".$row['Nome'].", ".$row['Prezzo_EUR'].")<br>";
			}
		}
	?>
	<p></p>
	<form action="update_Panino.php" method="POST">
		<input type="text" name="ID_Panino_old" placeholder="ID panino vecchio - Es. P01">
		<input type="text" name="ID_Panino_new" placeholder="ID panino nuovo - Es. P02">
		<input type="text" name="nome_new" placeholder="Nome nuovo - Es. Big Burger">
		<input type="text" name="prezzo_new" placeholder="Prezzo nuovo (EUR) - Es. 4.20">
		<button type="submit" name="submit">Modifica</button>
	</form>
	<p></p>
	<div class="vertical-menu">
		<a href="../../index.php" class=active>Torna alla Home</a>
	</div>
</body>
</html>