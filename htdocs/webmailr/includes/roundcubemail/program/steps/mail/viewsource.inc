<?php

/*
 +-----------------------------------------------------------------------+
 | program/steps/mail/viewsource.inc                                     |
 |                                                                       |
 | This file is part of the RoundCube Webmail client                     |
 | Copyright (C) 2005-2009, RoundCube Dev. - Switzerland                 |
 | Licensed under the GNU GPL                                            |
 |                                                                       |
 | PURPOSE:                                                              |
 |   Display a mail message similar as a usual mail application does     |
 |                                                                       |
 +-----------------------------------------------------------------------+
 | Author: Thomas Bruederli <roundcube@gmail.com>                        |
 +-----------------------------------------------------------------------+

 $Id: viewsource.inc,v 1.1 2011/08/01 19:22:03 eldy Exp $

*/

ob_end_clean();

// similar code as in program/steps/mail/get.inc
if ($uid = get_input_value('_uid', RCUBE_INPUT_GET))
{
  $headers = $IMAP->get_headers($uid);
  $charset = $headers->charset ? $headers->charset : $CONFIG['default_charset'];
  header("Content-Type: text/plain; charset={$charset}");

  if (!empty($_GET['_save'])) {
    $filename = ($headers->subject ? $IMAP->decode_header($headers->subject) : 'roundcube') . '.eml';
    $browser = new rcube_browser;
    
    if ($browser->ie && $browser->ver < 7)
      $filename = rawurlencode(abbreviate_string($filename, 55));
    else if ($browser->ie)
      $filename = rawurlencode($filename);
    else
      $filename = addcslashes($filename, '"');

    header("Content-Length: {$headers->size}");
    header("Content-Disposition: attachment; filename=\"$filename\"");
  }
  
  $IMAP->print_raw_body($uid);
}
else
{
  raise_error(array(
      'code' => 500,
      'type' => 'php',
      'file' => __FILE__, 'line' => __LINE__,
      'message' => 'Message UID '.$uid.' not found'),
    true, true);
}

exit;

