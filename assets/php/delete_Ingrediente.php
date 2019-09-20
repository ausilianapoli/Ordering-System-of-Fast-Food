<?php

	include_once 'dbh.php';

	if(isset($_POST['submit'])){
		$ingrediente = mysqli_real_escape_string($conn, $_POST['ID_Ingrediente']);

		$sql = "DELETE FROM ingrediente WHERE ID_Ingrediente='$ingrediente';";
		mysqli_query($conn, $sql);

		header("Location: ../php/success.php");
	}
	else{
		header("Location: ../php/error.php");
	}

?>