/** Possible outcomes of a file reading operation. More stuff*/
enum EST_read_status {
    /// The file was read in successfully
  read_ok		= make_status_int(rws_ok,	rwr_none,	0),
    /// The file exists but is not in the format specified
  read_format_error	= make_status_int(rws_failed,	rwr_format,	0),
    /// The file does not exist.
  read_not_found_error	= make_status_int(rws_failed,	rwr_existance,	0),
    /// An error occurred while reading
  read_error		= make_status_int(rws_failed,	rwr_unknown,	0)
};


/** Possible outcomes of a file writing operation */
enum EST_write_status {
    /// The file was written successfully
  write_ok		= make_status_int(rws_ok,	rwr_none,	0),
    /// The file was not written successfully
  write_fail		= make_status_int(rws_failed,	rwr_unknown,	0),
    /// The file was not written successfully
  write_error		= make_status_int(rws_failed,	rwr_unknown,	0),
    /// A valid file was created, but only some of the requested data is in there
  write_partial		= make_status_int(rws_partial,	rwr_unknown,	0)
};

/** Possible outcomes of a network connection operation */
enum EST_connect_status {
  /// Connection made.
  connect_ok		= make_status_int(rws_ok,	rwr_none,	0),
  /// Connection failed.
  connect_not_found_error = make_status_int(rws_failed,	rwr_existance,	0),
  /// Connection failed.
  connect_not_allowed_error = make_status_int(rws_failed, rwr_permission, 0),
  /// System failure of some kind
  connect_system_error = make_status_int(rws_failed, rwr_system, 0),
  /// The file was not written successfully
  connect_error		= make_status_int(rws_failed,	rwr_unknown,	0)
};

