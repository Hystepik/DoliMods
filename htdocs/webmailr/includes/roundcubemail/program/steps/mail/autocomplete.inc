<?php

/*
 +-----------------------------------------------------------------------+
 | program/steps/mail/autocomplete.inc                                   |
 |                                                                       |
 | This file is part of the RoundCube Webmail client                     |
 | Copyright (C) 2008-2010, RoundCube Dev Team                           |
 | Licensed under the GNU GPL                                            |
 |                                                                       |
 | PURPOSE:                                                              |
 |   Perform a search on configured address books for the address        |
 |   autocompletion of the message compose screen                        |
 +-----------------------------------------------------------------------+
 | Author: Thomas Bruederli <roundcube@gmail.com>                        |
 +-----------------------------------------------------------------------+

 $Id: autocomplete.inc,v 1.1 2011/08/01 19:22:03 eldy Exp $

*/

$MAXNUM = 15;
$book_types = (array) $RCMAIL->config->get('autocomplete_addressbooks', 'sql');

if ($RCMAIL->action == 'group-expand') {
  $abook = $RCMAIL->get_address_book(get_input_value('_source', RCUBE_INPUT_GPC));
  if ($gid = get_input_value('_gid', RCUBE_INPUT_GPC)) {
    $members = array();
    $abook->set_group($gid);
    $abook->set_pagesize(1000);  // TODO: limit number of group members by config
    $result = $abook->list_records(array('email','name'));
    while ($result && ($sql_arr = $result->iterate()))
      $members[] = format_email_recipient($sql_arr['email'], $sql_arr['name']);

    $OUTPUT->command('replace_group_recipients', $gid, join(', ', $members));
  }
}
else if ($book_types && $search = get_input_value('_search', RCUBE_INPUT_GPC, true)) {
  $contacts = array();
  $books_num = count($book_types);

  foreach ($book_types as $id) {
    $abook = $RCMAIL->get_address_book($id);
    $abook->set_pagesize($MAXNUM);

    if ($result = $abook->search(array('email','name'), $search, false, true, true, 'email')) {
      while ($sql_arr = $result->iterate()) {
        $contact = format_email_recipient($sql_arr['email'], $sql_arr['name']);
        // when we've got more than one book, we need to skip duplicates
        if ($books_num == 1 || !in_array($contact, $contacts)) {
          $contacts[] = $contact;
          if (count($contacts) >= $MAXNUM)
            break 2;
        }
      }
    }

    // also list matching contact groups
    if ($abook->groups) {
      foreach ($abook->list_groups($search) as $group) {
        $abook->reset();
        $abook->set_group($group['ID']);
        $result = $abook->count();

        if ($result->count) {
          $contacts[] = array('name' => $group['name'] . ' (' . intval($result->count) . ')', 'id' => $group['ID'], 'source' => $id);
          if (count($contacts) >= $MAXNUM)
            break;
        }
      }
    }
  }

  usort($contacts, 'contact_results_sort');
}

$OUTPUT->command('ksearch_query_results', $contacts, $search);
$OUTPUT->send();


function contact_results_sort($a, $b)
{
  $name_a = is_array($a) ? $a['name'] : $a;
  $name_b = is_array($b) ? $b['name'] : $b;
  return strcoll(trim($name_a, '" '), trim($name_b, '" '));
}

