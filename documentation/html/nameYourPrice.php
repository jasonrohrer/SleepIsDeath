<?php

$price = "0.00";
if( isset( $_REQUEST[ "price" ] ) ) {
    $price = $_REQUEST[ "price" ];
    }

$priceInCents = round( 100 * $price );

$referrer = "";
if( isset( $_REQUEST[ "referrer" ] ) ) {

    // pass it through without a regex filter
    // because we can't control its safety in the end anyway
    // (user can just edit URL sent to FastSpring).
    
    $referrer = urlencode( $_REQUEST[ "referrer" ] );
    }



header( "Location:https://sites.fastspring.com/jasonrohrer/instant/sleepisdeathticket?referrer=$referrer&tags=price_amount=$priceInCents");



?>