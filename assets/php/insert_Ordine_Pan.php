<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$prenotazione = mysqli_real_escape_string($conn, $_POST['ID_Prenotazione']);
		$panino = mysqli_real_escape_string($conn, $_POST['ID_Panino']);
		$quantita = mysqli_real_escape_string($conn, $_POST['Quantita']);
		$note = mysqli_real_escape_string($conn, $_POST['Note']);

		$sql = "INSERT INTO ordine_pan VALUES ($prenotazione, '$panino', $quantita, '$note');";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>