#!/bin/bash

# Test script for the zig-mcp-template server

SERVER="./zig-out/bin/zig-mcp-template"

echo "Testing MCP Server"
echo "=================="
echo

# Build first if needed
if [ ! -f "$SERVER" ]; then
    echo "Building server..."
    zig build
    echo
fi

# Test 1: Initialize
echo "Test 1: Initialize"
echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{}}}' | $SERVER | head -1
echo

# Test 2: Tools List
echo "Test 2: List Tools"
echo '{"jsonrpc":"2.0","id":2,"method":"tools/list","params":{}}' | $SERVER | head -1
echo

# Test 3: Roll Dice
echo "Test 3: Roll Dice (3d8)"
echo '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"roll-dice","arguments":{"notation":"3d8"}}}' | $SERVER | head -1
echo

# Test 4: Roll Dice (1d20)
echo "Test 4: Roll Dice (1d20)"
echo '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"roll-dice","arguments":{"notation":"1d20"}}}' | $SERVER | head -1
echo

# Test 5: Invalid dice notation
echo "Test 5: Invalid Dice Notation"
echo '{"jsonrpc":"2.0","id":5,"method":"tools/call","params":{"name":"roll-dice","arguments":{"notation":"invalid"}}}' | $SERVER | head -1
echo

# Test 6: Unknown method
echo "Test 6: Unknown Method"
echo '{"jsonrpc":"2.0","id":6,"method":"unknown","params":{}}' | $SERVER | head -1
echo

echo "=================="
echo "Tests Complete"
