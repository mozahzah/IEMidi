// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#import "IECore/IERenderer.h"

#if defined (__APPLE__)

#import <Cocoa/Cocoa.h>
#import <UserNotifications/UserNotifications.h>

@interface IEAppleStatusItem : NSObject<NSApplicationDelegate>
@property IERenderer* renderer;
@property (strong, nonatomic) NSStatusItem* statusItem;
@end

@implementation IEAppleStatusItem
- (id)initWithRenderer:(IERenderer*)Renderer
{
    self = [super init];
    if (self) 
    {
        self.renderer = Renderer;
        [self createStatusBarIcon];
    }    
    return self;
}

- (void)createStatusBarIcon
{
    NSStatusBar* statusBar = [NSStatusBar systemStatusBar];
    self.statusItem = [statusBar statusItemWithLength:NSSquareStatusItemLength];
    if (self.statusItem && self.renderer)
    {
        NSString* logoFilePath = [NSString stringWithUTF8String:self.renderer->GetIEIconPathString().c_str()];
        NSImage* icon = [[NSImage alloc] initWithContentsOfFile:logoFilePath];
        if (icon)
        {
            [icon setSize:NSMakeSize([statusBar thickness] - 5, [statusBar thickness] - 5)];
            [self.statusItem button].image = icon;
                
            NSMenu* menuItem = [[NSMenu alloc] initWithTitle:@"IEMidi"];
            [menuItem setMinimumWidth:200];
            [menuItem setAutoenablesItems:NO];

            NSView *menuTitleView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 200, 30)];
            NSTextField *menuTitleLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(10, 0, 190, 20)];
            NSFont *titleFont = [NSFont systemFontOfSize:13 weight:NSFontWeightSemibold];
            NSDictionary *titleAttributes = @{NSFontAttributeName: titleFont};
            NSAttributedString *titleAttribute = [[NSAttributedString alloc] initWithString:@"IEMidi" attributes:titleAttributes];
            [menuTitleLabel setAttributedStringValue:titleAttribute];
            [menuTitleLabel setEditable:NO];
            [menuTitleLabel setBezeled:NO];
            [menuTitleLabel setDrawsBackground:NO];
            [menuTitleView addSubview:menuTitleLabel];
            
            NSMenuItem* titleMenuItem = [[NSMenuItem alloc] initWithTitle:@"IEMidi" action:nil keyEquivalent:@""];
            [titleMenuItem setView:menuTitleView];
            [menuItem addItem:titleMenuItem];

            NSMenuItem *subTitleMenuItem = [[NSMenuItem alloc] init];
            [subTitleMenuItem setEnabled:NO];
            NSFont *subTitleFont = [NSFont menuFontOfSize:12];
            NSDictionary *subTitleAttributes = @{NSFontAttributeName: subTitleFont};
            NSAttributedString *subTitleAttribute = [[NSAttributedString alloc] initWithString:@"App State: Running" attributes:subTitleAttributes];
            subTitleMenuItem.attributedTitle = subTitleAttribute;
            [menuItem addItem:subTitleMenuItem];

            [menuItem addItem:[NSMenuItem separatorItem]];
            
            NSMenuItem* editMenuItem = [[NSMenuItem alloc] initWithTitle:@"Edit" action:@selector(edit:) keyEquivalent:@""];
            [editMenuItem setTarget:self];
            [menuItem addItem:editMenuItem];

            NSMenuItem* quitMenuItem = [[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(quit:) keyEquivalent:@""];
            [quitMenuItem setTarget:self];
            [menuItem addItem:quitMenuItem];

            [self.statusItem setMenu:menuItem];
        }
    }
}

- (void)edit:(id)sender
{
    if (self.renderer)
    {
        self.renderer->RestoreAppWindow();
    }
}

- (void)quit:(id)sender
{
    if (self.renderer)
    {
        self.renderer->RequestExit();
    }
}
@end

@interface IEAppleNotificationDelegate : NSObject <UNUserNotificationCenterDelegate>
@end

@implementation IEAppleNotificationDelegate
- (instancetype)init
{
    self = [super init];
    if (self)
    {
        UNUserNotificationCenter* notificationCenter = [UNUserNotificationCenter currentNotificationCenter];
        notificationCenter.delegate = self;

        [notificationCenter requestAuthorizationWithOptions : (UNAuthorizationOptionAlert | UNAuthorizationOptionSound | UNAuthorizationOptionBadge)
            completionHandler : ^ (BOOL granted, NSError * _Nullable error)
        {
            if (!granted)
            {
                NSLog(@"Notification permission not granted: %@", error.localizedDescription);
            }
        }];
    }
    return self;
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
    withCompletionHandler : (void(^)(UNNotificationPresentationOptions options))completionHandler
    {
        completionHandler(UNNotificationPresentationOptionBadge | UNNotificationPresentationOptionBanner | UNNotificationPresentationOptionSound);
    }
@end

/* EXTERN C Code */

extern "C" void InitializeIEAppleApp(IERenderer * Renderer)
{
    IEAppleStatusItem* IEStatusItem = [[IEAppleStatusItem alloc]initWithRenderer:Renderer];
    IEAppleNotificationDelegate *IENotificationDelegate = [[IEAppleNotificationDelegate alloc] init];
}

extern "C" void ShowRunningInBackgroundAppleNotification(IERenderer * Renderer)
{
    UNMutableNotificationContent *content = [[UNMutableNotificationContent alloc] init];
    content.title = @"IEMidi";
    content.body = @"IEMidi is running in the background";
    content.sound = [UNNotificationSound soundNamed:@"Purr"];

    NSString* uniqueIdentifier = [[NSUUID UUID]UUIDString];
    UNNotificationRequest* request = [UNNotificationRequest requestWithIdentifier : uniqueIdentifier
        content:content trigger:nil];

    [[UNUserNotificationCenter currentNotificationCenter]addNotificationRequest:request withCompletionHandler : ^ (NSError * _Nullable error)
    {
        if (error != nil)
        {
            NSLog(@"Error adding notification: %@", error.localizedDescription);
        }
        else
        {
            NSLog(@"Notification added successfully %@", uniqueIdentifier);
        }
    }];
}
#endif