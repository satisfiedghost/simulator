// Statuses for the simulator
// Functions can return these to the simulator to make it more state aware
// None: No change, function call was a no-op.
// Success: An operation was performed successfully.
// Impossible: A situation was encountered which should not be possible.
//             Indicates a possible issue with the simulator

enum class Status {
  None,
  Success,
  Impossible
};