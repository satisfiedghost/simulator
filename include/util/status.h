#pragma once
// Status

enum class Status {
  None,         ///<< No change occurred. This function call was a no-op for the system.
  Success,      ///<< An operation was performed successfully.
  Impossible,   ///<< A situation was encountered which should not be possible.
                ///<<   the system has corrected the errror.
  Inconsistent, ///<< A situation was encountered which should not be possible.
                ///<<   the system was unable to correct the error, and may be in a bad state.
  NotReady,     ///<< The requested action was unable to be performed, because a resource is not ready.
                ///<<    callers may e.g. busy-wait on this to change or repeatedly check in.
  Failure       ///<< The requested action failed, and the system may be in a bad state.
};