<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$prenotazione = mysqli_real_escape_string($conn, $_POST['ID_Prenotazione']);
		$panino = mysqli_real_escape_string($conn, $_POST['ID_Panino']);

		$sql = "DELETE FROM ordine_pan WHERE ID_Panino='$panino' AND ID_Prenotazione='$prenotazione';";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>