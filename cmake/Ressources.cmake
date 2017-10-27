# we don't want to copy if we're building in the source dir
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    # list of files for which we add a copy rule
    set(data_SHADOW ${Project_SOURCE_DIR}/models/deploy.prototxt)
    list(APPEND data_SHADOW ${Project_SOURCE_DIR}/models/train.caffemodel
      ${Project_SOURCE_DIR}/models/synset_words.txt)

    foreach(item IN LISTS data_SHADOW)
        message(STATUS ${item})
        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${item}" "${CMAKE_CURRENT_BINARY_DIR}/${item}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
        )
    endforeach()
endif()

# files are only copied if a target depends on them
add_custom_target(data-target ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/models/deploy.prototxt"
  "${CMAKE_CURRENT_BINARY_DIR}/models/train.caffemodel"
  "${CMAKE_CURRENT_BINARY_DIR}/models/synset_words.txt")