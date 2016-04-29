.. _logs:

logs
****

The logs class is used to interact with the log files created by the SysAdm server.

Every logs class request contains the following parameters:

+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| **Parameter**                   | **Value**     | **Description**                                                                                                      |
|                                 |               |                                                                                                                      |
+=================================+===============+======================================================================================================================+
| id                              |               | any unique value for the request; examples include a hash, checksum, or uuid                                         |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| name                            | logs          |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| namespace                       | rpc           |                                                                                                                      |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+
| action                          |               | supported actions include "read_logs"                                                                                |
|                                 |               |                                                                                                                      |
+---------------------------------+---------------+----------------------------------------------------------------------------------------------------------------------+

The rest of this section provides examples of the available *actions* for each type of request, along with their responses. 

.. index:: read_logs, logs

.. _Read Logs:

Read Logs
=========

The "read_logs" action can be used to display log entries. It supports the following optional arguments:

* **logs:** used to specify an array or string of log type(s). Valid types are "hostinfo", "dispatcher", "events-dispatcher","events-lifepreserver", and "events-state".

* **time_format:** used to specify the format for "start_time" and "end_time". Valid Formats are "time_t_seconds", "epoch_mseconds, "relative_[day/month/second]", or a
  `QDateTime String code <http://doc.qt.io/qt-5/qdatetime.html#fromString>`_.
  
* **start_time:** displays log entries that occurred after the time specified using a valid "time_format".

* **end_time:** displays log entries that occurred before the time specified using a valid "time_format".

If the "time_format" is not specified, or if the "start_time" or "end_time" are not defined, the end time will be the current date and time and the start time will be 12 hours previous.

If the "logs" argument is missing or empty, then all logs matching the search parameters will be returned.

For example, this input returns all log entries within the last hour:

.. code-block:: json

 {
 "action" : "read_logs",
 "time_format" : "relative_second",
 "start_time" : "-3600"
 }

And will return this format:

.. code-block:: json

 "args" : {
  "<log_file_type>" : {
    "<date_time_stamp>" : <message>,
    "<date_timo_stamp2>" : <message>
  }
 }

