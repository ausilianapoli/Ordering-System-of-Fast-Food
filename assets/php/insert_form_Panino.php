<!DOCTYPE html>
<html lang=ita>
<head><meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" type="text/css" href="../css/style.css">
	<title>La mia Panineria</title>
</head>
<body>
	<h2>Inserisci un nuovo panino</h2>
	<form action="insert_Panino.php" method="POST">
		<input type="text" name="ID_Panino" placeholder="ID Panino - Es. P01">
		<input type="text" name="nome" placeholder="Nome - Es. Hot Dog">
		<input type="text" name="prezzo" placeholder="Prezzo - Es. 2.50">
		<button type="submit" name="submit">Inserisci</button>
	</form>
	<p></p>
	<div class="vertical-menu">
		<a href="../../index.php" class=active>Torna alla Home</a>
	</div>
</body>
</html>