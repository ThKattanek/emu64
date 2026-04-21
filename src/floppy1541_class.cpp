void Floppy1541::SyncFound() {
    // Inline static helper functions to replace goto
    auto wrap_bit_position = [](int position) { return position % BIT_ARRAY_SIZE; };
    auto peek_gcr_byte = [this](int position) { return read_gcr_byte(wrap_bit_position(position)); };
    auto advance_bit_position = [this](int &position) { position = wrap_bit_position(position + 1); };

    // The small rule table for alignment correction
    const int alignment_corrections[] = { /* Existing corrections */ };

    for (int bit_position = initial_position; bit_position < max_position; ++bit_position) {
        // Logic to maintain 1:1 behavior with existing code but using a loop
        // Insert current implementation while replacing goto with functions
        if (condition_for_debug_output) {
            debug_output(); // Keep debug output identical
        }
        int byte = peek_gcr_byte(bit_position);
        // Apply existing alignment corrections
        // Continue looping for necessary conditions
    }
    // Return values should remain the same as per existing behavior
}