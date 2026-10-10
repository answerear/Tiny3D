/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include "T3DPlatformGuardScanner.h"

#include <fstream>
#include <sstream>
#include <stack>
#include <algorithm>


namespace Tiny3D
{
    //-------------------------------------------------------------------------

    PlatformGuardScanner::PlatformGuardScanner()
    {
        mKnownMacros.insert("T3D_OS_DESKTOP");
        mKnownMacros.insert("T3D_OS_WINDOWS");
        mKnownMacros.insert("T3D_OS_ANDROID");
        mKnownMacros.insert("T3D_OS_MOBILE");
        mKnownMacros.insert("T3D_OS_LINUX");
        mKnownMacros.insert("T3D_OS_OSX");
        mKnownMacros.insert("T3D_OS_IOS");
        mKnownMacros.insert("T3D_EDITOR");
    }

    //-------------------------------------------------------------------------

    bool PlatformGuardScanner::isKnownPlatformMacro(const String &macroName) const
    {
        return mKnownMacros.find(macroName) != mKnownMacros.end();
    }

    //-------------------------------------------------------------------------

    String PlatformGuardScanner::extractMacroFromDirective(const String &line) const
    {
        String trimmed = line;
        StringUtil::trim(trimmed);

        // Skip the leading '#'
        if (trimmed.empty() || trimmed[0] != '#')
            return "";

        trimmed = trimmed.substr(1);
        StringUtil::trim(trimmed);

        bool negated = false;
        String macroName;

        if (StringUtil::startsWith(trimmed, "ifdef", false))
        {
            // #ifdef MACRO
            String rest = trimmed.substr(5);
            StringUtil::trim(rest);
            macroName = rest;
        }
        else if (StringUtil::startsWith(trimmed, "ifndef", false))
        {
            // #ifndef MACRO → negated
            String rest = trimmed.substr(6);
            StringUtil::trim(rest);
            macroName = rest;
            negated = true;
        }
        else if (StringUtil::startsWith(trimmed, "if", false))
        {
            // #if defined(X) / #if defined X / #if !defined(X)
            String rest = trimmed.substr(2);
            StringUtil::trim(rest);

            // Check for negation: #if !defined(...)
            if (!rest.empty() && rest[0] == '!')
            {
                negated = true;
                rest = rest.substr(1);
                StringUtil::trim(rest);
            }

            if (StringUtil::startsWith(rest, "defined", false))
            {
                rest = rest.substr(7);
                StringUtil::trim(rest);

                if (!rest.empty() && rest[0] == '(')
                {
                    rest = rest.substr(1);
                    auto pos = rest.find(')');
                    if (pos != String::npos)
                    {
                        macroName = rest.substr(0, pos);
                    }
                }
                else
                {
                    std::istringstream iss(rest);
                    iss >> macroName;
                }
            }
            else
            {
                return "";
            }
        }
        else if (StringUtil::startsWith(trimmed, "elif", false))
        {
            // #elif defined(X) — treat similarly to #if
            String rest = trimmed.substr(4);
            StringUtil::trim(rest);

            if (!rest.empty() && rest[0] == '!')
            {
                negated = true;
                rest = rest.substr(1);
                StringUtil::trim(rest);
            }

            if (StringUtil::startsWith(rest, "defined", false))
            {
                rest = rest.substr(7);
                StringUtil::trim(rest);

                if (!rest.empty() && rest[0] == '(')
                {
                    rest = rest.substr(1);
                    auto pos = rest.find(')');
                    if (pos != String::npos)
                    {
                        macroName = rest.substr(0, pos);
                    }
                }
                else
                {
                    std::istringstream iss(rest);
                    iss >> macroName;
                }
            }
            else
            {
                return "";
            }
        }

        StringUtil::trim(macroName);

        if (macroName.empty())
            return "";

        if (!isKnownPlatformMacro(macroName))
            return "";

        if (negated)
            return "!" + macroName;

        return macroName;
    }

    //-------------------------------------------------------------------------

    void PlatformGuardScanner::scan(const String &filePath)
    {
        if (isScanned(filePath))
            return;

        std::ifstream ifs(filePath.c_str());
        if (!ifs.is_open())
        {
            RP_LOG_WARNING("PlatformGuardScanner: cannot open file [%s]", filePath.c_str());
            return;
        }

        struct PendingBlock
        {
            uint32_t startLine;
            String   condition;
            bool     isPlatformGuard;
        };

        std::stack<PendingBlock> blockStack;
        std::vector<GuardRange> ranges;

        std::string rawLine;
        uint32_t lineNum = 0;

        while (std::getline(ifs, rawLine))
        {
            lineNum++;

            String line(rawLine);
            String trimmed = line;
            StringUtil::trim(trimmed);

            if (trimmed.empty() || trimmed[0] != '#')
                continue;

            // Determine the directive type
            String afterHash = trimmed.substr(1);
            StringUtil::trim(afterHash);

            if (StringUtil::startsWith(afterHash, "if", false)
                && !StringUtil::startsWith(afterHash, "ifdef", false)
                && !StringUtil::startsWith(afterHash, "ifndef", false))
            {
                // #if ...
                String macro = extractMacroFromDirective(trimmed);
                PendingBlock block;
                block.startLine = lineNum;
                block.condition = macro;
                block.isPlatformGuard = !macro.empty();
                blockStack.push(block);
            }
            else if (StringUtil::startsWith(afterHash, "ifdef", false))
            {
                // #ifdef ...
                String macro = extractMacroFromDirective(trimmed);
                PendingBlock block;
                block.startLine = lineNum;
                block.condition = macro;
                block.isPlatformGuard = !macro.empty();
                blockStack.push(block);
            }
            else if (StringUtil::startsWith(afterHash, "ifndef", false))
            {
                // #ifndef ...
                String macro = extractMacroFromDirective(trimmed);
                PendingBlock block;
                block.startLine = lineNum;
                block.condition = macro;
                block.isPlatformGuard = !macro.empty();
                blockStack.push(block);
            }
            else if (StringUtil::startsWith(afterHash, "elif", false))
            {
                // #elif — close previous block, open new one
                if (!blockStack.empty())
                {
                    PendingBlock &top = blockStack.top();
                    if (top.isPlatformGuard)
                    {
                        GuardRange range;
                        range.startLine = top.startLine;
                        range.endLine = lineNum - 1;
                        range.condition = top.condition;
                        ranges.push_back(range);
                    }

                    String macro = extractMacroFromDirective(trimmed);
                    top.startLine = lineNum;
                    top.condition = macro;
                    top.isPlatformGuard = !macro.empty();
                }
            }
            else if (StringUtil::startsWith(afterHash, "else", false))
            {
                // #else — close previous block, open negated block
                if (!blockStack.empty())
                {
                    PendingBlock &top = blockStack.top();
                    if (top.isPlatformGuard)
                    {
                        GuardRange range;
                        range.startLine = top.startLine;
                        range.endLine = lineNum - 1;
                        range.condition = top.condition;
                        ranges.push_back(range);

                        // The #else branch gets the negated condition
                        String negated;
                        if (!top.condition.empty() && top.condition[0] == '!')
                            negated = top.condition.substr(1);
                        else
                            negated = "!" + top.condition;

                        top.startLine = lineNum;
                        top.condition = negated;
                        top.isPlatformGuard = true;
                    }
                    else
                    {
                        top.startLine = lineNum;
                    }
                }
            }
            else if (StringUtil::startsWith(afterHash, "endif", false))
            {
                // #endif — close current block
                if (!blockStack.empty())
                {
                    PendingBlock &top = blockStack.top();
                    if (top.isPlatformGuard)
                    {
                        GuardRange range;
                        range.startLine = top.startLine;
                        range.endLine = lineNum;
                        range.condition = top.condition;
                        ranges.push_back(range);
                    }
                    blockStack.pop();
                }
            }
        }

        ifs.close();

        mFileGuards[filePath] = std::move(ranges);
    }

    //-------------------------------------------------------------------------

    String PlatformGuardScanner::getGuardAtLine(const String &filePath, uint32_t line) const
    {
        auto itr = mFileGuards.find(filePath);
        if (itr == mFileGuards.end())
            return "";

        const auto &ranges = itr->second;

        // Find the innermost (most specific) guard containing this line.
        // Multiple guards may overlap due to nesting; pick the one with
        // the narrowest span (largest startLine).
        String bestGuard;
        uint32_t bestStart = 0;

        for (const auto &range : ranges)
        {
            if (line > range.startLine && line < range.endLine)
            {
                if (bestGuard.empty() || range.startLine > bestStart)
                {
                    bestGuard = range.condition;
                    bestStart = range.startLine;
                }
            }
        }

        return bestGuard;
    }

    //-------------------------------------------------------------------------

    bool PlatformGuardScanner::isScanned(const String &filePath) const
    {
        return mFileGuards.find(filePath) != mFileGuards.end();
    }
}
